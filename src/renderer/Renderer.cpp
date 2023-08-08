#include "renderer/Precomp.h"
#include "renderer.h"

#include "core/Kernel.h"
#include "core/Timer/Timer.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/File/File.h"
#include "core/Scheduler/Scheduler.h"

#include "gfx/device/device.h"
#include "gfx/Shader/ShaderManager.h"
#include "gfx/FrameGraph/FrameGraph.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Importer/TextureImporter.h"

#include "renderer/Pass/ImGui/ImGuiPass.h"
#include "renderer/Pass/ImGui/imguiAdapter.h"
#include "renderer/Importer/FBX/FBXImporter.h"
#include "renderer/Importer/SceneImporterData.h"
#include "renderer/Model/Mesh/MeshModel.h"
#include "renderer/Options/DisplayOptions.h"
#include "renderer/IGraphicInstance.h"
#include "renderer/Importer/TextureImporterData.h"
#include "renderer/View/View.h"
#include "renderer/View/Forward/ForwardView.h"

#include "imgui/imgui.h"

#include "shaders/driver/driver.hlsl.h"
#include "shaders/default/default.hlsl.h"
#include "Shaders/background/background.hlsl.h"

using namespace vg::core;
using namespace vg::gfx;
using namespace vg::renderer;

//--------------------------------------------------------------------------------------
IRenderer * CreateNew()
{
	return new Renderer("Renderer", nullptr);
}

//--------------------------------------------------------------------------------------
namespace vg::renderer
{
	#define VG_RENDERER_VERSION_MAJOR 0
	#define VG_RENDERER_VERSION_MINOR 12

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
    core::IProfiler * Renderer::GetProfiler() const
    {
        return Kernel::getProfiler();
    }

    //--------------------------------------------------------------------------------------
    IImGuiAdapter * Renderer::GetImGuiAdapter() const
    {
        return m_imgui;
    }

	//--------------------------------------------------------------------------------------
	core::IPlugin::Version Renderer::GetVersion() const
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
    bool Renderer::RegisterClasses()
    {
        IFactory * factory = Kernel::getFactory();

        // Register classes to auto-register the "Engine" module
        AutoRegisterClassInfo::registerClasses(*factory);

        if (IClassDesc * desc = factory->registerClassSingletonHelper(Renderer, "Renderer", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::UnregisterClasses()
    {
        IFactory * factory = Kernel::getFactory();
        return AutoRegisterClassInfo::unregisterClasses(*factory);
    }

    //--------------------------------------------------------------------------------------
    bool Renderer::registerProperties(IClassDesc & _desc)
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
        Kernel::setFactory(_singletons.factory);

        // Create device
		m_device.init(_params.device);

        // UI
        m_imgui = new ImGuiAdapter(_params.device.window, m_device);

        registerShaders();

        // Create passes not bound to a View
        m_imguiPass = new ImGuiPass();

        RegisterClasses();

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
        UnregisterClasses();

        deinitDefaultTextures();

        DisplayOptions * displayOptions = DisplayOptions::get();
        VG_SAFE_DELETE(displayOptions);

        VG_SAFE_RELEASE(m_mainView);
        for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        {
            auto & views = m_views[j];
            for (uint i = 0; i < views.size(); ++i)
                VG_SAFE_RELEASE(views[i]);
            views.clear();
        }

        VG_SAFE_DELETE(m_imguiPass);
        VG_SAFE_DELETE(m_imgui);

        m_frameGraph.release();

		m_device.deinit();
	}

    //--------------------------------------------------------------------------------------
    void Renderer::SetResized()
    {
        m_frameGraph.setResized();
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
    void Renderer::SetVSync(gfx::VSync mode)
    {
        m_device.setVSync(mode);
    }

    //--------------------------------------------------------------------------------------
    gfx::VSync Renderer::GetVSync() const
    {
        return m_device.getVSync();
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

            // Culling all views
            cullViews();

            // Framegraph
            {
                VG_PROFILE_CPU("Framegraph");

                m_frameGraph.importRenderTarget("Backbuffer", m_device.getBackbuffer(), float4(0,0,0,0), FrameGraph::Resource::InitState::Clear);
                m_frameGraph.setGraphOutput("Backbuffer");

                for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
                {
                    auto & views = m_views[j];
                    for (uint i = 0; i < views.count(); ++i)
                    {
                        auto * view = views[i];
                        if (nullptr != view && nullptr != view->getUniverse())
                            view->AddToFrameGraph(m_frameGraph);
                    }
                }

                RenderContext mainViewRenderContext;
                              mainViewRenderContext.m_view = m_mainView;

                m_frameGraph.addUserPass(mainViewRenderContext, m_imguiPass, "UIPass");

                m_frameGraph.setup(_dt);
                m_frameGraph.build();
                m_frameGraph.render();
            }
		}
		m_device.endFrame();
	}

    #pragma optimize("", off)

    //--------------------------------------------------------------------------------------
    // TODO: create additional views from Views (Shadowmaps, Cubemaps ...)
    //--------------------------------------------------------------------------------------
    void Renderer::cullViews()
    {
        core::Scheduler * jobScheduler = (core::Scheduler *)Kernel::getScheduler();

        // Perform culling foreach view (might want to split views later)
        uint jobStartCounter = 0;
        core::JobSync syncCull;
        for (uint j = 0; j < core::enumCount<gfx::ViewTarget>(); ++j)
        {
            const auto & views = m_views[j];
            for (uint i = 0; i < views.size(); ++i)
            {
                View * view = views[i];
                if (view)
                {
                    auto * job = view->getCullingJob();
                    jobScheduler->Start(job, &syncCull);
                    jobStartCounter++;
                }
            }
        }

        if (jobStartCounter > 0)
        {
            VG_PROFILE_CPU("syncCull");
            jobScheduler->Wait(syncCull);
        }
    }

    //--------------------------------------------------------------------------------------
    View * Renderer::getMainView() const
    {
        return m_mainView;
    }

	//--------------------------------------------------------------------------------------
	Texture * Renderer::getBackbuffer() const
	{
		return m_device.getBackbuffer();
	}

    //--------------------------------------------------------------------------------------
    const gfx::DeviceParams & Renderer::GetDeviceCreationParams() const
    {
        return m_device.getDeviceParams();
    }

    //--------------------------------------------------------------------------------------
    core::IObject * Renderer::GetDisplayOptions()
    {
        return DisplayOptions::get();
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * Renderer::CreateTexture(const gfx::TextureDesc & _texDesc, const core::string & _name)
    {
        auto tex = m_device.createTexture(_texDesc, _name);
        tex->setName(_name);
        return tex;
    }

    //--------------------------------------------------------------------------------------
    gfx::IView * Renderer::CreateView(gfx::CreateViewParams _params, const core::string & _name)
    {
        View * view;

        switch (_params.target)
        {
            default:
                view = new ForwardView(_params);
                break;

            case ViewTarget::Backbuffer:
                view = new View(_params);
                VG_ASSERT(m_mainView == nullptr, "Only one \"Main\" backbuffer view is supported");
                m_mainView = static_cast<View *>(view); // This is required for FrameGraph construction
                VG_SAFE_INCREASE_REFCOUNT(m_mainView);
                break;
        }
        
        view->setName(_name);
        return view;
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewID Renderer::AddView(gfx::IView * _view)
    {
        auto target = _view->GetViewID().target;
        auto & views = m_views[(uint)target];

        // Find hole or push_back
        VG_ASSERT(views.size() < (ViewIndex)-1);
        VG_SAFE_INCREASE_REFCOUNT(_view);

        for (uint i = 0; i < views.size(); ++i)
        {
            if (!views[i])
            {
                views[i] = (View*)_view;
                auto id = ViewID(target, i);
                _view->SetViewID(id);
                return id;
            }
        }

        auto index = (ViewIndex)views.count();
        ViewID id = ViewID(target, index);
        views.push_back((View *)_view);
        _view->SetViewID(id);
        return id;
    }

    //--------------------------------------------------------------------------------------
    const core::vector <IView *> Renderer::GetViews(gfx::ViewTarget _target) const
    {
        VG_ASSERT(_target < (gfx::ViewTarget)core::enumCount<gfx::ViewTarget>());
        return (const core::vector<IView *>&)m_views[(uint)_target];
    }

    //--------------------------------------------------------------------------------------
    void Renderer::RemoveView(gfx::ViewID _viewID)
    {
        // Keep holes so that the ViewID remains valid
        auto & views = m_views[(uint)_viewID.target];
        for (uint i = 0; i < views.size(); ++i)
        {
            if (views[i])
            {
                const auto id = views[i]->GetViewID();
                if (id.index == _viewID.index)
                    VG_SAFE_RELEASE(views[i]);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::IView * Renderer::GetView(gfx::ViewID _viewID) const
    {
        const auto & views = m_views[(uint)_viewID.target];
        if (_viewID.index < views.size())
            return views[_viewID.index];
        else
            return nullptr;
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

        if (TextureImporter::get()->importTextureData(_file, imported.desc, imported.texels))
        {
            imported.save(io::getCookedPath(_file));

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * Renderer::loadTexture(const core::string & _file)
    {
        TextureImporterData textureData;

        if (textureData.load(io::getCookedPath(_file)))
        {
            ITexture * texture = m_device.createTexture(textureData.desc, textureData.name, textureData.texels.data());
            return texture;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void Renderer::ReleaseAsync(core::IObject * _object)
    {
        m_device.releaseAsync((Object*)_object);
    }

    //--------------------------------------------------------------------------------------
    gfx::Texture * Renderer::getDefaultTexture(MaterialTextureType _type) const
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