#pragma once

#include "ShaderKey.h"
#include "ComputeShaderKey.h"
#include "gfx/IShaderManager.h"
#include "core/Scheduler/Mutex.h"

namespace vg::core
{
    class Blob;
}

namespace vg::gfx
{
    class Shader;
    class ShaderCompiler;
    class HLSLDesc;

    vg_enum_class(ShaderOptimizationLevel, core::u8,
        Unoptimized = 0,
        Optimized
    );

    class ShaderManager : public IShaderManager, public core::Singleton<ShaderManager>
    {
    public:
        const char * GetClassName() const final { return "ShaderManager"; }

        using OnShadersUpdatedCallbackFunc = void(*)(bool);

        ShaderManager(const core::string & _shaderRootPath, gfx::API _api);
        ~ShaderManager();

        core::uint                          GetShaderCompiledCount          () const final override;
        core::uint                          GetShaderWarningCount           () const final override;
        core::uint                          GetShaderErrorCount             () const final override;

        void                                setOnShadersUpdatedCallback     (OnShadersUpdatedCallbackFunc _onShadersUpdatedCallback);

        void                                loadFromCache                   ();
        void                                update                          (bool _firstUpdate = false);
        void                                applyUpdate                     (bool _firstUpdate = false);

        void                                registerHLSL                    (const HLSLDesc & _hlslDesc);
        bool                                initShaderKey                   (ShaderKey & _key, const core::string & _file, const core::string & _technique);
        bool                                initComputeShaderKey            (ComputeShaderKey & _key, const core::string & _file, const core::string & _technique);

        HLSLDesc *                          getHLSLDescriptor               (ShaderKey::File _file);

        core::Blob                          compile                         (API _api, const core::string & _file, const core::string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros);

        const core::string &                getShaderRootPath               () const { return m_shaderRootPath; }
        const core::vector<core::string> &  getShaderRootFolders            () const { return m_shaderRootFolders; }

        #if VG_SHADER_SOURCE_IN_MEMORY
        const core::string *                getShaderFilePathFromID         (core::uint _fileID, bool _mustExist = true) const;
        const core::string *                getShaderSource                 (const core::string & _path, bool _mustExist = true) const;
        void                                saveShaderSourceInMemory        ();
        #endif

        gfx::API                            getAPI                          () const;
        ShaderOptimizationLevel             getShaderOptimizationLevel      () const;

        core::mutex &                       getGraphicPipelineStateMutex    () { return m_graphicPipelineStateMutex; }
        core::mutex &                       getComputePipelineStateMutex    () { return m_computePipelineStateMutex; }

    private:
        void                                parseIncludes                   (core::uint _index, core::string dir, core::string _file, core::vector<core::string> & _includes, core::u64 & _crc, core::uint _depth = 0);
        static core::string                 fixFileLine                     (const core::string & _filename, const core::string & _warningAndErrorString);

    private:
        bool                                m_updateNeeded = false;
        core::string                        m_shaderRootPath;
        core::vector<core::string>          m_shaderRootFolders;
        ShaderCompiler *                    m_shaderCompiler = nullptr;
        gfx::API                            m_api;
        core::vector<HLSLDesc>              m_shaderFileDescriptors;
        core::uint                          m_compiledCount = 0;
        core::uint                          m_warningCount = 0;
        core::uint                          m_errorCount = 0;
        ShaderOptimizationLevel             m_optimizationLevel;

        #if VG_SHADER_SOURCE_IN_MEMORY
        core::unordered_map<core::string, core::string> m_shaderFilesHash;
        core::unordered_map<core::uint, core::string>   m_shaderFileIDToPath;
        core::uint                                      m_shaderFilesCounter = 0;
        #endif

        OnShadersUpdatedCallbackFunc        m_onShadersUpdatedCallbackFunc = nullptr;
        core::mutex                         m_graphicPipelineStateMutex;
        core::mutex                         m_computePipelineStateMutex;
    };
}