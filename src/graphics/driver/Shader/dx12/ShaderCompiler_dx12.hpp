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
    string readFile2(const string & _file)
    {
        using namespace std;

        ifstream file(_file.c_str(), ios::in | ios::binary | ios::ate);

        if (file.is_open())
        {
            ifstream::pos_type fileSize = file.tellg();
            if (fileSize < 0)
            {
                file.close();
            }
            else
            {
                file.seekg(0, ios::beg);
                core::vector<char> bytes(fileSize);
                file.read(&bytes[0], fileSize);
                file.close();
                return string(&bytes[0], fileSize);
            }
        }

        VG_ASSERT(file.is_open(), "Could not read file \"%s\"", _file.c_str());
        return string();
    }

    //--------------------------------------------------------------------------------------
    wstring string2wstring(const string & _string)
    {
        wchar_t result[1024];
        VG_ASSERT(_string.length() < countof(result));
        size_t s;
        mbstowcs_s(&s, result, countof(result), _string.c_str(), _string.length());
        return std::wstring(result);
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
    driver::Shader * ShaderCompiler::compile(const core::string & _file, const core::string & _entryPoint, ShaderStage _stage)
    {
        using namespace std;

        RETRY:

        VG_DEBUGPRINT("compiling %s shader \"%s\" from \"%s\"\n", asString(_stage).c_str(), _entryPoint.c_str(), _file.c_str());

        string source = readFile2(_file);

        IDxcBlobEncoding * dxcSource;
        VG_ASSERT_SUCCEEDED(m_d3d12dxcLibrary->CreateBlobWithEncodingFromPinned(source.c_str(), (uint)source.size(), CP_UTF8, &dxcSource));

        const wchar_t * pArgs[] =
        {
            //L"-WX",
        #ifdef VG_DEBUG
            L"-Zi",
            L"-Od"
        #else
            L"-O3"
        #endif
        };

        std::vector<DxcDefine> dxcDefines(0/*defines.size()*/);
        //for (size_t i = 0; i < defines.size(); ++i)
        //{
        //    DxcDefine& m = dxcDefines(i);
        //    m.Name = defines[i].first.c_str();
        //    m.Value = defines[i].second.c_str();
        //}

        wstring wfilename = string2wstring(_file);
        wstring wEntryPoint = string2wstring(_entryPoint);

        wstring wTargetProfile = string2wstring(string(getd3d12TargetProfile(_stage)));

        IDxcOperationResult * dxcCompileResult = nullptr;
        m_d3d12dxcCompiler->Compile(dxcSource, wfilename.c_str(), wEntryPoint.c_str(), wTargetProfile.c_str(), pArgs, (uint)countof(pArgs), dxcDefines.data(), (uint)dxcDefines.size(), m_d3d12dxcIncludeHandler, &dxcCompileResult);

        HRESULT hrCompilation;
        VG_ASSERT_SUCCEEDED(dxcCompileResult->GetStatus(&hrCompilation));

        IDxcBlobEncoding * dxcWarningAndErrors;
        VG_ASSERT_SUCCEEDED(dxcCompileResult->GetErrorBuffer(&dxcWarningAndErrors));

        VG_DEBUGPRINT("%s", dxcWarningAndErrors->GetBufferPointer());

        if (hrCompilation < 0)
        {
            const string title = _file;
            const string message = "Shader compilation failed:\n\n" + string((char*)dxcWarningAndErrors->GetBufferPointer());

            if (MessageBoxResult::Retry == messageBox(MessageBoxIcon::Error, MessageBoxType::RetryCancel, title, message))
            {
                VG_SAFE_RELEASE(dxcSource);
                VG_SAFE_RELEASE(dxcCompileResult);
                VG_SAFE_RELEASE(dxcWarningAndErrors);

                goto RETRY;
            }

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
}