namespace vg::graphics::driver::dx12
{
    //--------------------------------------------------------------------------------------
    ShaderCompiler::ShaderCompiler()
    {
        VG_ASSERT_SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_d3d12dxcLibrary)));
        VG_ASSERT_SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_d3d12dxcCompiler)));
        VG_ASSERT_SUCCEEDED(m_d3d12dxcLibrary->CreateIncludeHandler(&m_d3d12dxcIncludeHandler));
    }

    //--------------------------------------------------------------------------------------
    ShaderCompiler::~ShaderCompiler()
    {
        VG_SAFE_RELEASE(m_d3d12dxcIncludeHandler);
        VG_SAFE_RELEASE(m_d3d12dxcCompiler);
        VG_SAFE_RELEASE(m_d3d12dxcLibrary);
    }

    //--------------------------------------------------------------------------------------
    // Using the highest profiles dxil.dll can actually validate using Win10 SDK 10.0.17763.0 
    //--------------------------------------------------------------------------------------
    const char * getd3d12TargetProfile(ShaderStage _stage)
    {
        switch (_stage)
        {
            default:
                VG_ASSERT(false);

            case ShaderStage::Vertex:
                return "vs_6_1";

            case ShaderStage::Hull:
                return "hs_6_1";

            case ShaderStage::Domain:
                return "ds_6_1";

            case ShaderStage::Geometry:
                return "gs_6_1";

            case ShaderStage::Pixel:
                return "ps_6_1";

            case ShaderStage::Compute:
                return "cs_6_1";                
        }
    }

    //--------------------------------------------------------------------------------------
    driver::Shader * ShaderCompiler::compile(const core::string & _path, const core::string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros, string & _warningAndErrors)
    {
        string source;
        
        if (readFile(_path, source))
        {
            IDxcBlobEncoding * dxcSource;
            VG_ASSERT_SUCCEEDED(m_d3d12dxcLibrary->CreateBlobWithEncodingFromPinned(source.c_str(), (uint)source.size(), CP_UTF8, &dxcSource));

            const wchar_t * pArgs[] =
            {
                //L"-WX",
                #ifdef VG_DEBUG
                L"-Od",
                L"-Zi",
                L"-Qembed_debug",
                #else
                L"-O3"
                #endif
            };

            auto macros = _macros;

            macros.push_back({ "DX12", 1 });

            std::vector<DxcDefine> dxcDefines;
            for (const auto & macro : macros)
                dxcDefines.push_back({ _wcsdup(wstring_convert(macro.first).c_str()), _wcsdup(wstring_convert(to_string(macro.second)).c_str()) });

            const wstring wfilename = wstring_convert(_path);
            const wstring wEntryPoint = wstring_convert(_entryPoint);
            const wstring wTargetProfile = wstring_convert(string(getd3d12TargetProfile(_stage)));

            IDxcOperationResult * dxcCompileResult = nullptr;
            m_d3d12dxcCompiler->Compile(dxcSource, wfilename.c_str(), wEntryPoint.c_str(), wTargetProfile.c_str(), pArgs, (uint)countof(pArgs), dxcDefines.data(), (uint)dxcDefines.size(), m_d3d12dxcIncludeHandler, &dxcCompileResult);

            for (auto & dxcDef : dxcDefines)
            {
                free((void*)dxcDef.Name);
                free((void*)dxcDef.Value);
            }
            dxcDefines.clear();

            HRESULT hrCompilation;
            VG_ASSERT_SUCCEEDED(dxcCompileResult->GetStatus(&hrCompilation));

            IDxcBlobEncoding * dxcWarningAndErrors;
            VG_ASSERT_SUCCEEDED(dxcCompileResult->GetErrorBuffer(&dxcWarningAndErrors));

            if (nullptr != dxcWarningAndErrors->GetBufferPointer())
                VG_DEBUGPRINT("%s", dxcWarningAndErrors->GetBufferPointer());

            if (hrCompilation < 0)
            {
                const string message = "Shader compilation failed:\n\n" + string((char*)dxcWarningAndErrors->GetBufferPointer());

                _warningAndErrors += message;
                
                VG_SAFE_RELEASE(dxcSource);
                VG_SAFE_RELEASE(dxcCompileResult);
                VG_SAFE_RELEASE(dxcWarningAndErrors);

                return nullptr;
            }

            IDxcBlob * dxcCompiledBlob = nullptr;
            dxcCompileResult->GetResult(&dxcCompiledBlob);

            core::Blob blob(dxcCompiledBlob->GetBufferPointer(), (uint)dxcCompiledBlob->GetBufferSize());

            auto * shader = new driver::Shader(blob);

            VG_SAFE_RELEASE(dxcSource);
            VG_SAFE_RELEASE(dxcCompileResult);
            VG_SAFE_RELEASE(dxcWarningAndErrors);
            VG_SAFE_RELEASE(dxcCompiledBlob);

            return shader;
        }

        return nullptr;
    }
}