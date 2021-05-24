#include "graphics/renderer/Precomp.h"
#include "renderer.h"

#include "core/Kernel.h"
#include "core/Timer/Timer.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/ISector.h"
#include "core/File/File.h"
#include "graphics/driver/device/device.h"
#include "graphics/driver/Shader/ShaderManager.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"
#include "graphics/driver/Profiler/Profiler.h"
#include "graphics/driver/Resource/Texture.h"
#include "graphics/driver/Importer/TextureImporter.h"
#include "graphics/renderer/Imgui/imguiAdapter.h"
#include "graphics/renderer/Pass/ImguiPass.h"
#include "graphics/renderer/Pass/BackgroundPass.h"
#include "graphics/renderer/Pass/TestPass3D.h"
#include "graphics/renderer/Pass/TestPass2D.h"
#include "graphics/renderer/Pass/PostProcessPass.h"
#include "graphics/renderer/View/View.h"
#include "graphics/renderer/Importer/FBX/FBXImporter.h"
#include "graphics/renderer/Importer/SceneImporterData.h"
#include "graphics/renderer/Model/Mesh/MeshModel.h"
#include "graphics/renderer/Options/DisplayOptions.h"
#include "graphics/renderer/IGraphicInstance.h"
#include "graphics/renderer/Importer/TextureImporterData.h"

#include "imgui/imgui.h"

#include "shaders/driver/driver.hlsl.h"
#include "shaders/default/default.hlsl.h"
#include "Shaders/background/background.hlsl.h"

using namespace vg::core;
using namespace vg::graphics::driver;
using namespace vg::graphics::renderer;

//--------------------------------------------------------------------------------------
IRenderer * CreateNew()
{
	return new Renderer("Renderer", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::graphics::renderer
{
	#define VG_RENDERER_VERSION_MAJOR 0
	#define VG_RENDERER_VERSION_MINOR 1

    #ifdef _WIN32
    //--------------------------------------------------------------------------------------
    LRESULT CALLBACK Renderer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;

        return 0;
    }
    #endif

    //--------------------------------------------------------------------------------------
    core::IProfiler * Renderer::getProfilerInstance() const
    {
        return Kernel::getProfiler();
    }

    //--------------------------------------------------------------------------------------
    IImmediateGUI * Renderer::getImmediateGUI() const
    {
        return m_imgui;
    }

	//--------------------------------------------------------------------------------------
	core::IPlugin::Version Renderer::getVersion() const
	{
		return { VG_RENDERER_VERSION_MAJOR, VG_RENDERER_VERSION_MINOR };
	}

	//--------------------------------------------------------------------------------------
	Renderer::Renderer(const core::string & _name, core::IObject * _parent) :
        IRenderer(_name, _parent),
		m_device(*(new Device())),
		m_frameGraph(*(new FrameGraph())),
        m_fbxImporter(new FBXImporter())
	{
        // Profiler instance has to be created by the graphic engine so as to profile the GPU too
        Kernel::setProfiler(new Profiler());
	}

	//--------------------------------------------------------------------------------------
	Renderer::~Renderer()
	{
        VG_SAFE_DELETE(m_fbxImporter);
		m_device.release();
        IProfiler * profiler = Kernel::getProfiler();
        VG_SAFE_DELETE(profiler);
	}

    //--------------------------------------------------------------------------------------
    bool Renderer::registerClasses()
    {
        IObjectFactory * factory = Kernel::getObjectFactory();

        // Register classes to auto-register the "Engine" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (IObjectDescriptor * desc = factory->registerClassSingletonHelper(Renderer, "Renderer", IObjectDescriptor::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::unregisterClasses()
    {
        IObjectFactory * factory = Kernel::getObjectFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::registerProperties(IObjectDescriptor & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

	//--------------------------------------------------------------------------------------
	void Renderer::init(const RendererParams & _params, core::Singletons & _singletons)
	{
        Timer::init();

        // Singletons created by the renderer
        _singletons.profiler = Kernel::getProfiler();

        // Singletons used by the renderer
        Kernel::setScheduler(_singletons.scheduler);
        Kernel::setInput(_singletons.input);
        Kernel::setObjectFactory(_singletons.factory);

        // Create device
		m_device.init(_params.device);

        // UI
        m_imgui = new ImguiAdapter(_params.device.window, m_device);

        registerShaders();

        // Create passes
        m_backgroundPass = new BackgroundPass();
        m_testPass3D = new TestPass3D();
        m_testPass2D = new TestPass2D();
        m_postProcessPass = new PostProcessPass();
        m_imguiPass = new ImguiPass();

        registerClasses();

        DisplayOptions * displayOptions = new DisplayOptions("DisplayOptions", this);

        initDefaultTextures();
	}

    //--------------------------------------------------------------------------------------
    void Renderer::registerShaders()
    {
        auto * sm = ShaderManager::get();

        sm->registerHLSL(DriverHLSLDesc());
        sm->registerHLSL(DefaultHLSLDesc());
        sm->registerHLSL(BackgroundHLSLDesc());

        sm->update();
    }

	//--------------------------------------------------------------------------------------
	void Renderer::deinit()
	{
        unregisterClasses();

        deinitDefaultTextures();

        DisplayOptions * displayOptions = DisplayOptions::get();
        VG_SAFE_DELETE(displayOptions);

        VG_SAFE_RELEASE(m_view);

        VG_SAFE_DELETE(m_backgroundPass);
        VG_SAFE_DELETE(m_testPass3D);
        VG_SAFE_DELETE(m_testPass2D);
        VG_SAFE_DELETE(m_postProcessPass);
        VG_SAFE_DELETE(m_imguiPass);
        VG_SAFE_DELETE(m_imgui);

        m_frameGraph.release();

		m_device.deinit();
	}

    //--------------------------------------------------------------------------------------
    void Renderer::resize(core::uint _width, core::uint _height)
    {
        m_device.waitGPUIdle();
        m_device.resize(_width, _height);
        m_frameGraph.destroyTransientResources();
    }

    //--------------------------------------------------------------------------------------
    core::uint2 Renderer::getBackbufferSize() const
    {
        const auto & desc = getBackbuffer()->getTexDesc();
        return { desc.width, desc.height };
    }

    //--------------------------------------------------------------------------------------
    void Renderer::updateShaders()
    {
        m_device.getShaderManager()->update();
    }

    //--------------------------------------------------------------------------------------
    void Renderer::waitGPUIdle()
    {
        m_device.waitGPUIdle();
    }

	//--------------------------------------------------------------------------------------
	void Renderer::runOneFrame(double _dt)
	{
        if (m_device.isMinimized())
        {
            Sleep(1);
            return;
        }

        VG_PROFILE_CPU("Renderer");

		m_device.beginFrame();
		{
            m_imgui->beginFrame();

            // Culling
            {
                // TODO:
            }

            // Framegraph
            {
                VG_PROFILE_CPU("Framegraph");

                m_frameGraph.importRenderTarget("Backbuffer", m_device.getBackbuffer(), float4(0,0,0,0), FrameGraph::Resource::InitState::Clear);
                m_frameGraph.setGraphOutput("Backbuffer");

                m_frameGraph.addUserPass(m_backgroundPass, "BackgroundPass");
                m_frameGraph.addUserPass(m_testPass3D, "TestPass3D");
                //m_frameGraph.addUserPass(m_testPass2D, "TestPass2D");
                m_frameGraph.addUserPass(m_postProcessPass, "PostProcessPass");
                m_frameGraph.addUserPass(m_imguiPass, "UIPass");

                m_frameGraph.setup(_dt);
                m_frameGraph.build();
                m_frameGraph.render();
            }
		}
		m_device.endFrame();
	}

    //--------------------------------------------------------------------------------------
    View * Renderer::getView() const
    {
        return m_view;
    }

	//--------------------------------------------------------------------------------------
	Texture * Renderer::getBackbuffer() const
	{
		return m_device.getBackbuffer();
	}

    //--------------------------------------------------------------------------------------
    IView * Renderer::createView(const CreateViewParams & _params)
    {
        return new View(_params);
    }

    //--------------------------------------------------------------------------------------
    void Renderer::setView(IView * _view)
    {
        if (_view != m_view)
        {
            VG_SAFE_INCREASE_REFCOUNT(_view);
            VG_SAFE_RELEASE(m_view);
            m_view = static_cast<View*>(_view);
        }
    }
    
    //--------------------------------------------------------------------------------------
    bool Renderer::cookMeshModel(const core::string & _file)
    {
        SceneImporterData imported;

        if (FBXImporter::get()->importFBX(_file, imported))
        {
            if (imported.meshes.size() > 0)
            {
                const auto & mesh = imported.meshes[0];
                mesh.save(io::getCookedPath(_file));
            }

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    IMeshModel * Renderer::loadMeshModel(const core::string & _file)
    {
        MeshImporterData meshData;

        if (meshData.load(io::getCookedPath(_file)))
        {
            IMeshModel * meshModel = MeshModel::createFromImporterData(meshData);
            return meshModel;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::cookTexture(const core::string & _file)
    {
        TextureImporterData imported;
        imported.name = _file;

        if (TextureImporter::get()->importTextureData(_file, imported.desc, imported.mips))
        {
            imported.save(io::getCookedPath(_file));

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    driver::ITexture * Renderer::loadTexture(const core::string & _file)
    {
        TextureImporterData textureData;

        if (textureData.load(io::getCookedPath(_file)))
        {
            ITexture * texture = m_device.createTexture(textureData.desc, textureData.name, textureData.mips[0].buffer.data());
            return texture;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    driver::Texture * Renderer::getDefaultTexture(MaterialTextureType _type) const
    {
        return m_defaultTextures[asInteger(_type)];
    }

    //--------------------------------------------------------------------------------------
    void Renderer::initDefaultTextures()
    {
        m_defaultTextures.resize(enumCount<MaterialTextureType>());

        for (uint t = 0; t < m_defaultTextures.size(); ++t)
        {
            const auto type = (MaterialTextureType)t;
            u32 data;
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case MaterialTextureType::Albedo:
                    data = 0xFFBBBBBB;
                    break;

                case MaterialTextureType::Normal:
                    data = 0xFFFF7F7F;
                    break;
            }

            TextureDesc texDesc = TextureDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::None, TextureType::Texture2D, PixelFormat::R8G8B8A8_unorm, TextureFlags::None, 1, 1);
            string name = "Default_" + asString(type);
            m_defaultTextures[t] = m_device.createTexture(texDesc, name.c_str(), &data);
        }        
    }

    //--------------------------------------------------------------------------------------
    void Renderer::deinitDefaultTextures()
    {
        for (uint t = 0; t < m_defaultTextures.size(); ++t)
            VG_SAFE_RELEASE(m_defaultTextures[t]);

        m_defaultTextures.clear();
    }
}