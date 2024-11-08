#include "dxc/inc/dxcapi.h"
#include "gfx/Shader/ShaderManager.h"
#include "core/File/File.h"

#pragma push_macro("new")
#undef new
#include "atlbase.h"
#pragma pop_macro("new")

#include "atlcomcli.h"

#define CUSTOM_DXC_INCLUDE_HANDLER 1

#if CUSTOM_DXC_INCLUDE_HANDLER

#include <wrl/client.h>
#include "core/string/string.h"

class CustomIncludeHandler : public IDxcIncludeHandler
{
public:
    HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob ** ppIncludeSource) override
    {
        using namespace Microsoft::WRL;
        using namespace vg::gfx;

        if (nullptr == m_dxcUtils)
            VG_VERIFY_SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_dxcUtils.GetAddressOf())));

        ComPtr<IDxcBlobEncoding> pEncoding;

        // wstring to string
        std::string path = string_convert(pFilename);

        // Behaviour changed in DirectShaderCompiler 1.8.2405.17:
        // include paths are now passed to IDxcIncludeHandler::LoadSource normalized to the operating system's path style. 
        // i.e. On Windows "/" is replaced with "\".
        // https://github.com/microsoft/DirectXShaderCompiler/issues/6669
        std::replace(path.begin(), path.end(), '\\', '/');

        const auto shaderManager = ShaderManager::get();
        const auto & shaderFolder = shaderManager->getShaderRootPath();
        const auto & rootFolders = shaderManager->getShaderRootFolders();

        bool found = false;
        for (uint i = 0; i < rootFolders.size() && !found; ++i)
        {
            const string shaderIncludeFolder = rootFolders[i];

            auto lastShaderFolder = tolower(path).rfind(shaderIncludeFolder+"/");

            if (string::npos != lastShaderFolder)
            {
                lastShaderFolder += shaderIncludeFolder.length() + 1;
                path = path.substr(lastShaderFolder);
                path = "./" + shaderFolder + shaderIncludeFolder + "/" + path;
            }
        }

        #if VG_SHADER_SOURCE_IN_MEMORY
        const string * pSource = ShaderManager::get()->getShaderSource(path);
        HRESULT hr = m_dxcUtils->CreateBlob(pSource->data(), (uint)pSource->size(), CP_UTF8, pEncoding.GetAddressOf());
        #else
        wstring filename = wstring_convert(path);
        HRESULT hr = m_dxcUtils->LoadFile(filename.c_str(), nullptr, pEncoding.GetAddressOf());
        #endif

        VG_ASSERT(SUCCEEDED(hr), "CustomIncludeHandler could not open #include file \"%s\"", path.c_str());
        if (SUCCEEDED(hr))
        {
            m_includedFiles.insert(path);
            *ppIncludeSource = pEncoding.Detach();
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR * __RPC_FAR * ppvObject) override { return E_NOINTERFACE; }
    ULONG STDMETHODCALLTYPE AddRef(void) override 
    { 
        return m_refCount.fetch_add(1) + 1;
    }

    ULONG STDMETHODCALLTYPE Release(void) override 
    { 
        const u32 prev = m_refCount.fetch_sub(1);
        if (1 == prev)
            delete this;
        return prev - 1;
    }

    ~CustomIncludeHandler()
    {
        if (m_dxcUtils)
            m_dxcUtils->Release();
    }

    atomic<u32>                         m_refCount = 1;
    std::unordered_set<std::string>     m_includedFiles;
    Microsoft::WRL::ComPtr<IDxcUtils>   m_dxcUtils;
};
#endif

namespace vg::gfx::dxc
{
    //--------------------------------------------------------------------------------------
    ShaderCompiler::ShaderCompiler()
    {
        VG_VERIFY_SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_d3d12dxcLibrary)));
        VG_VERIFY_SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_d3d12dxcCompiler)));

		CComPtr<IDxcVersionInfo3> pCompilerVersion3;
		m_d3d12dxcCompiler->QueryInterface(&pCompilerVersion3);

		if (nullptr != pCompilerVersion3)
		{
			CComHeapPtr<char> VersionString;
			VG_VERIFY_SUCCEEDED(pCompilerVersion3->GetCustomVersionString(&VersionString));
			VG_INFO("[Shader] Using DXC Shader Compiler Version %s", VersionString);
		}
		else
		{
			CComPtr<IDxcVersionInfo> pCompilerVersion;
			m_d3d12dxcCompiler->QueryInterface(&pCompilerVersion);

			if (pCompilerVersion)
			{
				UINT32 uCompilerMajor = 0;
				UINT32 uCompilerMinor = 0;
				VG_VERIFY_SUCCEEDED(pCompilerVersion->GetVersion(&uCompilerMajor, &uCompilerMinor));
				VG_INFO("[Shader] Using DXC Shader Compiler Version %u.%u ", uCompilerMajor, uCompilerMinor);
			}
		}		

        #if CUSTOM_DXC_INCLUDE_HANDLER
        m_d3d12dxcIncludeHandler = new CustomIncludeHandler();
        #else
        VG_VERIFY_SUCCEEDED(m_d3d12dxcLibrary->CreateIncludeHandler(&m_d3d12dxcIncludeHandler));
        #endif
    }

    //--------------------------------------------------------------------------------------
    ShaderCompiler::~ShaderCompiler()
    {
        VG_SAFE_RELEASE(m_d3d12dxcCompiler);
        VG_SAFE_RELEASE(m_d3d12dxcLibrary);
        VG_SAFE_RELEASE(m_d3d12dxcIncludeHandler);
    }

    //--------------------------------------------------------------------------------------
    string ShaderCompiler::getDXCTargetProfile(ShaderStage _stage) const
    {
        const auto & caps = Device::get()->getDeviceCaps();

        string prefix;
        switch (_stage)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_stage);
                break;

            case ShaderStage::Vertex:
                prefix = "vs";
                break;

            case ShaderStage::Hull:
                prefix = "hs";
                break;

            case ShaderStage::Domain:
                prefix = "ds";
                break;

            case ShaderStage::Geometry:
                prefix = "gs";
                break;

            case ShaderStage::Pixel:
                prefix = "ps";
                break;

            case ShaderStage::Compute:
                prefix = "cs";
                break;
        }

        string suffix;
        switch (caps.shaderModel)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(caps.shaderModel);
                break;

            case ShaderModel::SM_5_1:
                suffix = "5_1";
                break;

            case ShaderModel::SM_6_0:
                suffix = "6_0";
                break;

            case ShaderModel::SM_6_1:
                suffix = "6_1";
                break;

            case ShaderModel::SM_6_2:
                suffix = "6_2";
                break;

            case ShaderModel::SM_6_3:
                suffix = "6_3";
                break;

            case ShaderModel::SM_6_4:
                suffix = "6_4";
                break;

            case ShaderModel::SM_6_5:
                suffix = "6_5";
                break;

            case ShaderModel::SM_6_6:
                suffix = "6_6";
                break;

            case ShaderModel::SM_6_7:
                suffix = "6_7";
                break;
        }

        return fmt::sprintf("%s_%s", prefix, suffix);
    }

    //--------------------------------------------------------------------------------------
    gfx::Shader * ShaderCompiler::compile(API _api, const core::string & _path, const core::string & _entryPoint, ShaderStage _stage, const core::vector<core::pair<core::string, core::uint>> & _macros, string & _warningAndErrors)
    {
        #if VG_SHADER_SOURCE_IN_MEMORY
        const string * pSource = ShaderManager::get()->getShaderSource(_path);
        if (pSource)
        {
            IDxcBlobEncoding * dxcSource;
            VG_VERIFY_SUCCEEDED(m_d3d12dxcLibrary->CreateBlobWithEncodingFromPinned((*pSource).c_str(), (uint)(*pSource).size(), CP_UTF8, &dxcSource));

        #else

        string source;
        if (io::readFile(_path, source))
        {
            IDxcBlobEncoding * dxcSource;
            VG_VERIFY_SUCCEEDED(m_d3d12dxcLibrary->CreateBlobWithEncodingFromPinned(source.c_str(), (uint)source.size(), CP_UTF8, &dxcSource));
        #endif

            vector<wchar_t*> args;
            auto macros = _macros;

            args.push_back((wchar_t *)L"-HV 2021");

            #ifdef VG_DEBUG
            args.push_back((wchar_t*)L"-Od");
            args.push_back((wchar_t*)L"-Zi");
            args.push_back((wchar_t*)L"-Qembed_debug");
            #else
            args.push_back((wchar_t*)L"-O3");
            #endif

            switch (_api)
            {
                case API::Vulkan:
                    macros.push_back({ "VG_VULKAN", 1 });
                    args.push_back((wchar_t *)L"-spirv");
                    args.push_back((wchar_t *)L"-fvk-use-dx-layout");
                    args.push_back((wchar_t *)L"-fvk-use-dx-position-w");
                    break;

                case API::DirectX12:
                    macros.push_back({ "VG_DX12", 1 });
                    break;
                default:

                    VG_ASSERT(false, "Unhandled API \"%s\" (%u)", asString(_api), _api);
                    break;
            }

            std::vector<DxcDefine> dxcDefines;
            for (const auto & macro : macros)
                dxcDefines.push_back({ _wcsdup(wstring_convert(macro.first).c_str()), _wcsdup(wstring_convert(to_string(macro.second)).c_str()) });

            const wstring wfilename = wstring_convert(_path);
            const wstring wEntryPoint = wstring_convert(_entryPoint);
            const wstring wTargetProfile = wstring_convert(getDXCTargetProfile(_stage));

            IDxcOperationResult * dxcCompileResult = nullptr;
            m_d3d12dxcCompiler->Compile(dxcSource, wfilename.c_str(), wEntryPoint.c_str(), wTargetProfile.c_str(), (LPCWSTR*)args.data(), (uint)args.size(), dxcDefines.data(), (uint)dxcDefines.size(), m_d3d12dxcIncludeHandler, &dxcCompileResult);

            for (auto & dxcDef : dxcDefines)
            {
                free((void*)dxcDef.Name);
                free((void*)dxcDef.Value);
            }
            dxcDefines.clear();

            HRESULT hrCompilation;
            VG_VERIFY_SUCCEEDED(dxcCompileResult->GetStatus(&hrCompilation));

            IDxcBlobEncoding * dxcWarningAndErrors;
            VG_VERIFY_SUCCEEDED(dxcCompileResult->GetErrorBuffer(&dxcWarningAndErrors));

            const char * warningAndErrorBuffer = (const char*)dxcWarningAndErrors->GetBufferPointer();
            string warningAndErrors; 
            if (nullptr != warningAndErrorBuffer) 
                warningAndErrors = warningAndErrorBuffer;

            VG_ASSERT(core::io::getRootDirectory() == core::io::getCurrentWorkingDirectory());

            if (hrCompilation < 0)
            {
                _warningAndErrors += warningAndErrors;

                VG_SAFE_RELEASE(dxcSource);
                VG_SAFE_RELEASE(dxcCompileResult);
                VG_SAFE_RELEASE(dxcWarningAndErrors);

                return nullptr;
            }
            else
            {
                if (!warningAndErrors.empty()) 
                {
                    //if (warningAndErrors._Starts_with("In file included from"))
                    //{
                    //    auto endOfLoc = warningAndErrors.find(":\n");
                    //    string loc = warningAndErrors.substr(0, endOfLoc);
                    //    warningAndErrors = warningAndErrors.substr(endOfLoc + strlen(":\n"));
                    //}
                    //
                    //const string message = "Warning compiling shader:\n" + core::io::getRootDirectory() + "/" + warningAndErrors;
                    //_warningAndErrors += message;

                    _warningAndErrors += warningAndErrors;
                }
            }

            IDxcBlob * dxcCompiledBlob = nullptr;
            dxcCompileResult->GetResult(&dxcCompiledBlob);

            core::Blob blob(dxcCompiledBlob->GetBufferPointer(), (uint)dxcCompiledBlob->GetBufferSize());

            auto * shader = new gfx::Shader(blob);

            VG_SAFE_RELEASE(dxcSource);
            VG_SAFE_RELEASE(dxcCompileResult);
            VG_SAFE_RELEASE(dxcWarningAndErrors);
            VG_SAFE_RELEASE(dxcCompiledBlob);

            return shader;
        }

        return nullptr;
    }
}