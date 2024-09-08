#include "ShaderManager.h"
#include "core/File/File.h"
#include "core/string/string.h"
#include "core/Math/Math.h"
#include "core/Timer/Timer.h"

using namespace vg::core;

#define VG_SHADER_PARSING_VERBOSITY 0

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    ShaderManager::ShaderManager(const core::string & _shaderRootPath) :
        m_shaderRootPath(_shaderRootPath)
    {
        // Path must end with '\' or '/'
        if (m_shaderRootPath.back() != '/' && m_shaderRootPath.back() != '\\')
            m_shaderRootPath += "/";

        // TODO: read dir to root folders names used #include ?
        m_shaderRootFolders.push_back("system");
    
        m_shaderCompiler = new ShaderCompiler();
    }

    //--------------------------------------------------------------------------------------
    ShaderManager::~ShaderManager()
    {
        VG_SAFE_DELETE(m_shaderCompiler);
    }

    //--------------------------------------------------------------------------------------
    core::uint ShaderManager::GetShaderCompiledCount() const
    {
        return m_compiledCount;
    }
    //--------------------------------------------------------------------------------------
    core::uint ShaderManager::GetShaderWarningCount() const
    {
        return m_warningCount;
    }
    //--------------------------------------------------------------------------------------
    core::uint ShaderManager::GetShaderErrorCount() const
    {
        return m_errorCount;
    }

    //--------------------------------------------------------------------------------------
    void ShaderManager::registerHLSL(const HLSLDesc & _hlslDesc)
    {
        m_shaderFileDescriptors.push_back(_hlslDesc);
    }

    //--------------------------------------------------------------------------------------
    bool ShaderManager::initShaderKey(ShaderKey & _key, const core::string & _file, const core::string & _technique)
    {
        for (uint i = 0; i < m_shaderFileDescriptors.size(); ++i)
        {
            const HLSLDesc & desc = m_shaderFileDescriptors[i];
        
            if (!_file.compare(desc.getFile()))  
            {
                _key.file = i;

                const auto & techniques = desc.getTechniques();
                for (uint j = 0; j < techniques.size(); ++j)
                {
                    const auto & technique = techniques[j];

                    if (!_technique.compare(technique.name))
                    {
                        _key.vs = technique.vs;
                        _key.hs = technique.hs;
                        _key.ds = technique.ds;
                        _key.gs = technique.gs;
                        _key.ps = technique.ps;
                        _key.flags = technique.flags;

                        return true;
                    }
                }

                VG_ASSERT(false, "Graphic Technique \"%s\" not found in file \"%s\"", _technique.c_str(), _file.c_str());
                return false;
            }
        }

        VG_ASSERT(false, "File \"%s\" not found", _file.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ShaderManager::initComputeShaderKey(ComputeShaderKey & _key, const core::string & _file, const core::string & _technique)
    {
        for (uint i = 0; i < m_shaderFileDescriptors.size(); ++i)
        {
            const HLSLDesc & desc = m_shaderFileDescriptors[i];

            if (!_file.compare(desc.getFile()))
            {
                _key.file = i;

                const auto & techniques = desc.getTechniques();
                for (uint j = 0; j < techniques.size(); ++j)
                {
                    const auto & technique = techniques[j];

                    if (!_technique.compare(technique.name))
                    {
                        _key.cs = technique.cs;
                        _key.flags = technique.flags;

                        return true;
                    }
                }

                VG_ASSERT(false, "Compute Technique \"%s\" not found in file \"%s\"", _technique.c_str(), _file.c_str());
                return false;
            }
        }

        VG_ASSERT(false, "File \"%s\" not found", _file.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc * ShaderManager::getHLSLDescriptor(ShaderKey::File _file)
    {
        return &m_shaderFileDescriptors[_file];
    }

    //--------------------------------------------------------------------------------------
    // Fix the warning or error line number so that visual studio opens correct file when double-clicked
    // 
    // before : 
    // data/shaders/lighting/deferredLighting.hlsl:40:1: error: use of undeclared identifier 'caca'
    // 
    // after : 
    // D:/GitHub/vimontgames/vgframework/data/shaders/lighting/deferredLighting.hlsl(40): error: use of undeclared identifier 'caca'
    //--------------------------------------------------------------------------------------
    core::string ShaderManager::fixFileLine(const core::string & _filename, const core::string & _warningAndErrorString)
    {
        const auto root = core::io::getRootDirectory();
        auto lines = getLines(_warningAndErrorString);

        string result;
        for (uint i = 0; i < lines.size(); ++i)
        {
            string line = lines[i];
            if (line.length() > 0)
            {
                size_t pos = 0;
                bool first = true;
                while ((pos = line.find(':', pos)) != std::string::npos)
                {
                    if (pos < line.size())
                    {
                        if (std::isdigit(line[pos + 1]))
                        {
                            size_t endPos = line.find_first_not_of("0123456789", pos + 1);
                            std::string lineNumber = line.substr(pos + 1, endPos - pos - 1);

                            if (first)
                                line.replace(pos, endPos - pos, "(" + lineNumber + ")");
                            else
                                line.replace(pos, endPos - pos, "");

                            first = false;
                        }
                        else
                        {
                            line.replace(pos, 1, "");
                        }
                    }
                    pos += 1; 
                }

                string folder = "data";
                if (line._Starts_with(folder))
                    line.replace(0, folder.length(), root + "/" + folder);

                string shad = "shaders";
                string sys = "system";

                auto syspos = line.find(sys);
                if (string::npos != syspos)
                {
                    auto shadpos = line.find(shad);

                    if (syspos != shadpos + shad.length() + 1)
                    {
                        line.replace(shadpos + shad.length(), syspos - (shadpos + shad.length()) - 1, "");
                    }
                }

                result += line + "\n";
            }
        }

        return result;
    }

    //--------------------------------------------------------------------------------------
    Shader * ShaderManager::compile(API _api, const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, const vector<pair<string, uint>> & _macros)
    {
        RETRY:
        const auto startLoad = Timer::getTick();

        string warningAndErrors;
        Shader * shader = m_shaderCompiler->compile(_api, m_shaderRootPath + _file, _entryPoint, _stage, _macros, warningAndErrors);

        if (shader)
            m_compiledCount++;
        else
            m_errorCount++;
        
        string header = _file + "\n" + _entryPoint + " (" + asString(_stage) + " shader)\n";
        for (auto & macro : _macros)
            header += macro.first + " ";
        header += "\n";

        if (!warningAndErrors.empty())
        {
            warningAndErrors = fixFileLine(_file, warningAndErrors);

            if (!shader)
                VG_ERROR("[Shader] Error compiling %s Shader \"%s\" \n%s", asString(_stage).c_str(), _entryPoint.c_str(), warningAndErrors.c_str());
            else
            {
                m_warningCount++;
                VG_WARNING("[Shader] Warning compiling %s Shader \"%s\" \n%s", asString(_stage).c_str(), _entryPoint.c_str(), warningAndErrors.c_str());
            }
        }

        if (shader) 
        {
            string msg = fmt::sprintf("[Shader] Successfully compiled %s Shader \"%s\"", asString(_stage).c_str(), _entryPoint.c_str());

            if (_macros.size() > 0)
            {
                msg += " (";
                for (uint i = 0; i < _macros.size(); ++i)
                {
                    if (i > 0)
                        msg += " | ";
                    msg += _macros[i].first;
                }
                msg += ")";
            }
            VG_INFO("%s in %.2f ms", msg.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
        }
        else
        {
            warningAndErrors = header + warningAndErrors + "\nPress \"Yes\" to retry";

            if (warningAndErrors.length() > 2048)
                warningAndErrors = warningAndErrors.substr(0, 2048) + "\n\n[...]";

            switch (messageBox(MessageBoxIcon::Error, MessageBoxType::YesNoCancel, "Shader compilation failed", warningAndErrors.c_str()))
            {
                case MessageBoxResult::Yes:
                    update();
                    applyUpdate();
                    goto RETRY;

                default:
                    break;
            }
        }

        return shader;
    }

    //--------------------------------------------------------------------------------------
    void ShaderManager::parseIncludes(uint _index, string dir, string _file, vector<string> & _includes, u64 & _crc, uint _depth)
    {
        vector<string> paths;
        paths.push_back(_file);             // absolute path
        paths.push_back("Shaders/" + _file);
        paths.push_back(dir + "/" + _file); // path relative to current folder

        bool exists = false;

        for (uint i = 0; i < paths.size() && !exists; ++i)
        {
            string fullpath = paths[i];

            #if VG_SHADER_SOURCE_IN_MEMORY
            const string * pSource = getShaderSource("data/" + fullpath, false);
            if (pSource)
            {
                string & contents = (string&)*pSource;
            #else
            string contents;
            if (io::readFile("data/" + fullpath, contents, false))
            {
            #endif
                exists = true;

                if (_includes.end() == std::find(_includes.begin(), _includes.end(), fullpath))
                {
                    _includes.push_back(fullpath);
                    const u64 crc = computeCRC64(contents.c_str());
                    _crc ^= crc;

                    #if VG_SHADER_PARSING_VERBOSITY > 1
                    if (_depth)
                        VG_DEBUGPRINT("  + ");
                    else
                        VG_DEBUGPRINT("#%u: ", _index);

                    VG_DEBUGPRINT("\"%s\"\n", fullpath.c_str());
                    #endif

                    size_t cur = 0;
                    auto includeOffset = contents.find("#include", cur);

                    while (string::npos != includeOffset)
                    {
                        auto quoteBeginOffset = contents.find("\"", includeOffset + strlen("#include"));

                        if (string::npos != quoteBeginOffset)
                        {
                            auto quoteEndOffset = contents.find("\"", quoteBeginOffset + 1);

                            string path = contents.substr(quoteBeginOffset + 1, quoteEndOffset - quoteBeginOffset - 1);
                            path = tolower(path);

                            string dir = io::getFileDir(fullpath);
                            parseIncludes(_index, dir, path, _includes, _crc, ++_depth);

                            cur = quoteEndOffset;
                            includeOffset = contents.find("#include", cur);
                        }
                    }
                }
            }
        }

        if (!exists)
        {
            string msg = fmt::sprintf("Could not open file \"%s\" using paths:\n", _file);
            for (uint i = 0; i < paths.size(); ++i)
                msg += fmt::sprintf("paths[%u] = \"data/%s\";\n", i, paths[i]);

            VG_ASSERT(exists, msg.c_str());
        }
    }

    //--------------------------------------------------------------------------------------
    void ShaderManager::update(bool _forceUpdate)
    {
        if (_forceUpdate)
            applyUpdate(true);
        else
            m_updateNeeded = true;
    }

    struct HLSLFile
    {
        string name;
        string folder;
    };

    //--------------------------------------------------------------------------------------
    void getHLSLFilesInFolder(vector<HLSLFile> & _hlslFiles, const string & _root, string _subFolder = "")
    {
        auto files = io::getFilesInFolder(fmt::sprintf("%s/%s", _root, _subFolder));

        for (auto file : files)
        {
            if (file.isFolder)
            {
                if (file.name != "." && file.name != "..")
                    getHLSLFilesInFolder(_hlslFiles, _root, _subFolder + file.name);
            }
            else
            {
                if (io::fileHasExtension(file.name, ".hlsl") || io::fileHasExtension(file.name, ".hlsli"))
                {
                    auto & hlslFile = _hlslFiles.emplace_back();
                    hlslFile.name = file.name;
                    hlslFile.folder = _subFolder;
                }
            }
        }
    };

    #if VG_SHADER_SOURCE_IN_MEMORY
    //--------------------------------------------------------------------------------------
    // Build a hash 'm_shaderSourceHash' of all .hlsl files to compile from
    //--------------------------------------------------------------------------------------
    void ShaderManager::saveShaderSourceInMemory()
    {
        const auto start = Timer::getTick(); 

        vector<HLSLFile> hlslFiles;
        getHLSLFilesInFolder(hlslFiles, getShaderRootPath());

        m_shaderFilesHash.clear();
        m_shaderFileIDToPath.clear();
        m_shaderFilesCounter = 0;

        // used to generate unique string variable for each string in source
        uint string_counter = 0;

        // Now we have all the .hlsl files, let's preprocess them
        for (auto & file : hlslFiles)
        {
            string path = fmt::sprintf("%s%s/%s", getShaderRootPath(), file.folder, file.name);
            string source;
            if (io::readFile(path, source))
            {
                // "patch" the string used in file to turn them from "Foo" into {'F','o','o'}
                //DebugPrint("abc"); ==> static uint temp[] = { 'a','b','c' }; DebugPrint(temp);

                auto processLine = [](const string & _line, string & _source, uint & _string_counter, uint _fileID)
                {
                    //VG_DEBUGPRINT("%s\n", _line.c_str());

                    static const string stringFuncs[] =
                    {
                        "VG_DEBUGPRINT",
                        "VG_INFO",
                        "VG_WARNING",
                        "VG_ERROR",
                        "VG_ASSERT"
                    };

                    size_t stringFunc = string::npos;

                    for (uint i = 0; i < countof(stringFuncs); ++i)
                    {
                        stringFunc = _line.find(stringFuncs[i]);
                        if(string::npos != stringFunc)
                            break;
                    }
                    
                    if (string::npos != stringFunc)
                    {
                        auto strBegin = _line.find("\"");
                        if (string::npos != strBegin)
                        {
                            auto strEnd = _line.find("\"", strBegin + 1); 
                            if (string::npos != strEnd)
                            {
                                string str = _line.substr(strBegin + 1, strEnd - strBegin - 1);
                                string name = fmt::sprintf("_tmpstr_%u", _string_counter);
                                string temp = fmt::sprintf("static uint %s[] = {", name);

                                const auto len = str.length();
                                for (uint i = 0; i < len; ++i)
                                {
                                    if (i > 0)
                                        temp += ",";
                                    temp += fmt::sprintf("'%c'", str[i]);
                                }
                                temp += "}; ";

                                temp += fmt::sprintf("g_File = %u; ", _fileID);

                                _string_counter++;
                                
                                auto line = _line;
                                line.replace(strBegin, strEnd - strBegin + 1, name);
                                line.insert(stringFunc, temp);

                                _source.replace(_source.find(_line), _line.length(), line);
                            }
                        }
                    }
                };

                m_shaderFilesCounter++;
                uint fileID = m_shaderFilesCounter;

                size_t start = 0;
                size_t end;
                while (1) 
                {
                    string line;

                    auto endN = source.find("\n", start);
                    auto endR = source.find("\r", start);

                    end = string::npos;
                    if (string::npos != endN && string::npos != endR)
                        end = min(endN, endR);
                    else if (string::npos != endN)
                        end = endN;
                    else if (string::npos != endR)
                        end = endR;

                    if ((end = source.find("\n", start)) == string::npos) 
                    {
                        if (!(line = source.substr(start)).empty())
                            processLine(line, source, string_counter, fileID);
                        break;
                    }

                    line = source.substr(start, end - start);
                    processLine(line, source, string_counter, fileID);
                    start = end + 1;
                }

                m_shaderFilesHash.insert(std::pair(tolower(path), source));
                m_shaderFileIDToPath.insert(std::pair(fileID, tolower(path)));
            }
        }

        VG_INFO("[Shader] Copy %u shader files to memory in %.2f ms", hlslFiles.size(), Timer::getEnlapsedTime(start, Timer::getTick()));
    }

    //--------------------------------------------------------------------------------------
    const core::string * ShaderManager::getShaderFilePathFromID(core::uint _fileID, bool _mustExist) const
    {
        auto it = m_shaderFileIDToPath.find(_fileID);
        if (m_shaderFileIDToPath.end() != it)
            return &it->second;
        VG_ASSERT(!_mustExist || m_shaderFileIDToPath.end() != it, "Could not find shader file %u", _fileID);
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    const core::string * ShaderManager::getShaderSource(const core::string & _path, bool _mustExist) const
    {
        // clean path
        string path = tolower(_path);
        const string curDir = "./";
        auto prefix = _path.find_first_of(curDir);
        if (0 == prefix)
            path = path.substr(curDir.length());

        auto it = m_shaderFilesHash.find(path);
        VG_ASSERT(!_mustExist || m_shaderFilesHash.end() != it, "Could not read file \"%s\" from shaderSourceHash", path.c_str());
        if (m_shaderFilesHash.end() != it)
            return &it->second;

        return nullptr;
    }
    #endif

    //--------------------------------------------------------------------------------------
    void ShaderManager::applyUpdate(bool _forceUpdate)
    {
        if (m_updateNeeded || _forceUpdate)
        {
            #if VG_SHADER_SOURCE_IN_MEMORY
            saveShaderSourceInMemory();
            #endif

            //VG_INFO("[Shader] Update Shaders");

            Device * device = Device::get();

            const auto start = Timer::getTick();

            m_compiledCount = 0;
            m_warningCount = 0;
            m_errorCount = 0;

            uint updated = 0;

            for (uint i = 0; i < m_shaderFileDescriptors.size(); ++i)
            {
                vector<string> filenames;

                auto & desc = m_shaderFileDescriptors[i];
                const string & file = desc.getFile();

                u64 oldCRC = desc.getCRC();
                u64 newCRC = 0;
                parseIncludes(i, "", "shaders/" + file, filenames, newCRC);

                if (newCRC != oldCRC)
                {
                    if (0 != oldCRC)
                        VG_WARNING("[Shader] File \"%s\" updated (old CRC = 0x%016llu, new CRC = 0x%016llu)", file.c_str(), oldCRC, newCRC);

                    // delete the shaders
                    desc.reset();
                    
                    // delete the pso (TODO: L1 cache in cmdlist and L2 global cache using mutex)
                    device->resetShaders(ShaderKey::File(i));

                    ++updated;

                    desc.setCRC(newCRC);
                }
                else
                {
                    //VG_INFO("[Shader] File \"%s\" is up-to-date (CRC = 0x%016llu)", file.c_str(), oldCRC);
                }
            }

            VG_INFO("[Shader] %u/%u shader file(s) changed", updated, m_shaderFileDescriptors.size());
        }

        m_updateNeeded = false;
    }
}