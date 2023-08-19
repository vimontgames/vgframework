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

                VG_ASSERT(false, "Technique \"%s\" not found in file \"%s\"", _technique.c_str(), _file.c_str());
                return false;
            }
        }

        VG_ASSERT("File \"%s\" not found", _file.c_str());
        return false;
    }

    //--------------------------------------------------------------------------------------
    HLSLDesc * ShaderManager::getHLSLDescriptor(ShaderKey::File _file)
    {
        return &m_shaderFileDescriptors[_file];
    }

    //--------------------------------------------------------------------------------------
    Shader * ShaderManager::compile(API _api, const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, const vector<pair<string, uint>> & _macros)
    {
        RETRY:

        string msg = "[Shader] Compile " + asString(_stage) + " Shader \"" + _entryPoint + "\"";
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
        VG_LOG(Level::Info, msg.c_str());

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
            m_warningCount++;
            if (!shader)
                VG_LOG(Level::Error, "[Shader] %s", warningAndErrors.c_str());
            else
                VG_LOG(Level::Warning, "[Shader] %s", warningAndErrors.c_str());
        }

        if (!shader)
        {
            warningAndErrors = header + warningAndErrors + "\nPress \"Yes\" to retry";

            switch (messageBox(MessageBoxIcon::Error, MessageBoxType::YesNoCancel, "Shader compilation failed", warningAndErrors.c_str()))
            {
                case MessageBoxResult::Yes:
                    goto RETRY;

                default:
                    break;
            }
        }

        return shader;
    }

    //--------------------------------------------------------------------------------------
    void parseIncludes(uint _index, string dir, string _file, vector<string> & _includes, u64 & _crc, uint _depth = 0)
    {
        vector<string> paths;
        paths.push_back(_file);             // absolute path
        paths.push_back("Shaders/" + _file);
        paths.push_back(dir + "/" + _file); // path relative to current folder

        bool exists = false;

        for (uint i = 0; i < paths.size() && !exists; ++i)
        {
            string fullpath = paths[i];

            string contents;
            if (io::readFile("data/" + fullpath, contents, false))
            {
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
            string msg = "Could not open file " + _file + " using paths:\n";
            for (uint i = 0; i < paths.size(); ++i)
                msg += "paths["+ to_string(i) +"] = \"" + paths[i] + "\";\n";

            VG_ASSERT(exists, msg.c_str());
        }
    }

    //--------------------------------------------------------------------------------------
    void ShaderManager::update()
    {
        m_updateNeeded = true;
    }

    //--------------------------------------------------------------------------------------
    void ShaderManager::applyUpdate()
    {
        if (m_updateNeeded)
        {
            //VG_LOG(Level::Info, "[Shader] Update Shaders");

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
                    VG_LOG(Level::Warning, "[Shader] File \"%s\" is not up-to-date (old CRC = 0x%016llu, new CRC = 0x%016llu)", file.c_str(), oldCRC, newCRC);

                    // delete the shaders
                    desc.reset();
                    
                    // delete the pso (TODO: L1 cache in cmdlist and L2 global cache using mutex)
                    device->resetShaders(ShaderKey::File(i));

                    ++updated;

                    desc.setCRC(newCRC);
                }
                else
                {
                    //VG_LOG(Level::Info, "[Shader] File \"%s\" is up-to-date (CRC = 0x%016llu)", file.c_str(), oldCRC);
                }
            }

            VG_LOG(Level::Info, "[Shader] %u/%u shader(s) changed", updated, m_shaderFileDescriptors.size());
        }

        m_updateNeeded = false;
    }
}