#include "ShaderManager.h"
#include "core/File/File.h"
#include "core/string/string.h"
#include "core/Math/Math.h"

using namespace vg::core;

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    ShaderManager::ShaderManager(const core::string & _shaderRootPath) :
        m_shaderRootPath(_shaderRootPath)
    {
        // Path must end with '\' or '/'
        if (m_shaderRootPath.back() != '/' && m_shaderRootPath.back() != '\\')
            m_shaderRootPath += "/";
    
        m_shaderCompiler = new ShaderCompiler();
    }

    //--------------------------------------------------------------------------------------
    ShaderManager::~ShaderManager()
    {
        VG_SAFE_DELETE(m_shaderCompiler);
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

                    VG_ASSERT("Technique \"%s\" not found in file \"%s\"", _technique.c_str(), _file.c_str());
                    return false;
                }
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
    Shader * ShaderManager::compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, const vector<pair<string, uint>> & _macros)
    {
        RETRY:

        string warningAndErrors;

        Shader * shader = m_shaderCompiler->compile(m_shaderRootPath + _file, _entryPoint, _stage, _macros, warningAndErrors);

        if (!shader)
        {
            string header = _file + "\n" + _entryPoint + " (" + asString(_stage) + " shader)\n";
            for (auto & macro : _macros)
                header += macro.first + " ";
            header += "\n";

            VG_DEBUGPRINT(warningAndErrors.c_str());

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
        string contents;
        string fullpath = dir.empty() ? _file : dir + "/" + _file;
        if (readFile("data/shaders/" + fullpath, contents))
        {
            if (_includes.end() == std::find(_includes.begin(), _includes.end(), fullpath))
            {
                _includes.push_back(fullpath);
                const u64 crc = computeCRC64(contents.c_str());
                _crc ^= crc;

                if (_depth)
                    VG_DEBUGPRINT("  + ");
                else
                    VG_DEBUGPRINT("#%u: ", _index);

                VG_DEBUGPRINT("\"%s\"\n", fullpath.c_str());

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

                        string dir = getFileDir(fullpath);
                        parseIncludes(_index, dir, path, _includes, _crc, ++_depth);

                        cur = quoteEndOffset;
                        includeOffset = contents.find("#include", cur);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void ShaderManager::update()
    {
        Device * device = Device::get();
        device->waitGPUIdle();

        for (uint i = 0; i < m_shaderFileDescriptors.size(); ++i)
        {
            vector<string> filenames;

            auto & desc = m_shaderFileDescriptors[i];
            const string & file = desc.getFile();

            u64 oldCRC = desc.getCRC();
            u64 newCRC = 0;
            parseIncludes(i, "", file, filenames, newCRC);

            if (newCRC != oldCRC)
            {
                if (0 != oldCRC)
                {
                    VG_DEBUGPRINT("File \"%s\" is not up-to-date (old CRC = 0x%016llu, new CRC = 0x%016llu)\n", file.c_str(), oldCRC, newCRC);
                
                    // delete the shaders
                    desc.resetShaders();

                    // delete the pso (TODO: L1 cache in cmdlist and L2 global cache using mutex)
                    device->resetShaders(ShaderKey::File(i));
                }

                desc.setCRC(newCRC);
            }
            else
                VG_DEBUGPRINT("File \"%s\" is up-to-date (CRC = 0x%016llu)\n", file.c_str(), oldCRC);
        }
    }
}