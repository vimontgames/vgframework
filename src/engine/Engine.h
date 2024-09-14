#pragma once

#include "engine/IEngine.h"
#include "core/IScheduler.h"
#include "core/Singleton/Singleton.h"
#include "core/Resource/Resource.h"

namespace vg::core
{
    class Entity;
}

namespace vg::gfx
{
    class IView;
}

namespace vg::renderer
{
    class IRendererOptions;
}

namespace vg::physics
{
    class IPhysicsOptions;
}

namespace vg::audio
{
    class IAudioOptions;
}

namespace vg::engine
{
    vg_enum_class(EngineJobType, core::u8,
        Animation
    );

    class FreeCam;
    class ResourceManager;
    class WorldResource;
    class Selection;
    class World;
    class Scene;

	class Engine : public IEngine, public core::Singleton<Engine>
	{
	public:
        using super = IEngine;

		IPlugin::Version				        GetVersion	        () const final override;

										        Engine		        (const core::string & _name, core::IObject * _parent);
										        ~Engine		        ();

        const Time &                            GetTime             () const final override;

        const char *                            GetClassName        () const final { return "Engine"; }
        bool                                    RegisterClasses     () override;
        bool                                    UnregisterClasses   ();
        static bool                             registerProperties  (core::IClassDesc & _desc);
        
        void                                    OnPropertyChanged   (IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        static bool                             createProject       (core::IObject * _engine);
        static bool                             LoadGame         (core::IObject * _engine);
        static bool                             saveProject         (core::IObject * _engine);

        #ifdef _WIN32
        LRESULT CALLBACK                        WndProc             (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final;
        #endif

		void							        Init		        (const EngineCreationParams & _params, core::Singletons & _singletons) final;
		void							        Deinit		        () override;

        bool                                    IsPlaying           () const final override;
        bool                                    IsPaused            () const final override;

        void                                    Play                () final override;
        void                                    Stop                () final override;
        void                                    Pause               () final override;
        void                                    Resume              () final override;

        void                                    StartInPlayMode     (bool _enable) final override;

        void                                    Quit                () final override;
        bool                                    IsQuitting          () const final override;

        bool                                    LoadGame            (const core::string & _path) final;
        bool                                    UnloadGame          () final;
        IGame *                                 GetGame             () const final;

        core::IWorld *                          GetMainWorld        () const final override;
        const core::vector<core::IWorld *> &    GetWorlds           () const final override;

		void							        RunOneFrame	        () final override;
        void                                    FlushLoading        () final override;

        core::uint2                             GetScreenSize       () const final;

        #if VG_ENABLE_EDITOR
        editor::IEditor *                       GetEditor           () const final override;
        #endif

		renderer::IRenderer *	                GetRenderer	        () const final override;
        physics::IPhysics *                     GetPhysics          () const final override;
        audio::IAudio *                         GetAudio            () const final override;
        engine::IResourceManager *              GetResourceManager  () const final override;
        core::ISelection *                      GetSelection        () const final override;

        bool                                    CreateWorld         (const core::string & _filename) final override;
        bool                                    SaveWorld           () final override;
        bool                                    SaveWorldAs         (const core::string & _filename) final override;
        bool                                    LoadWorld           (const core::string & _filename) final override;
        IWorldResource *                        GetWorldResource    () final override;

        bool                                    SaveScene           (core::IResource * _sceneRes) final override;

        IEngineOptions *                        GetOptions          () final override;

        void                                    onResourceLoaded    (core::IResource * _resource) final override;
        void                                    onResourceUnloaded  (core::IResource * _resource) final override;

        core::JobSync *                         getJobSync          (EngineJobType _jobSync) { return &m_jobSync[core::asInteger(_jobSync)]; }

    public:
        renderer::IRendererOptions *            getRendererOptions  () const;
        physics::IPhysicsOptions *              getPhysicsOptions   () const;
        audio::IAudioOptions *                  getAudioOptions     () const;

        VG_INLINE const Time &                  getTime             () const;

        void                                    registerWorld       (World * _world);
        void                                    unregisterWorld     (World * _world);

    protected:
        void                                    updateDt            ();
        void                                    toggleFullscreen    ();

        VG_INLINE bool                          isPlaying           () const;
        VG_INLINE bool                          isPaused            () const;

        void                                    play                ();
        void                                    stop                ();
        void                                    pause               ();
        void                                    resume              ();

	private:
        bool                                    m_isPlaying         : 1;
        bool                                    m_isPaused          : 1;
        bool                                    m_startInPlayMode   : 1;
        bool                                    m_quit              : 1;
        IGame *                                 m_game              = nullptr;
        WorldResource *                         m_worldResource     = nullptr;

        #if VG_ENABLE_EDITOR
        editor::IEditor *                       m_editor            = nullptr;
        #endif

		renderer::IRenderer *	                m_renderer          = nullptr;
        physics::IPhysics *                     m_physics           = nullptr;
        audio::IAudio *                         m_audio             = nullptr;
        ResourceManager *                       m_resourceManager   = nullptr;
        Selection *                             m_selection         = nullptr;
        core::JobSync                           m_jobSync[core::enumCount<EngineJobType>()];
        core::vector<core::IWorld *>            m_worlds;

        struct TimeInternal
        {
            TimeInternal()
            {
                reset();
            }

            void reset()
            {
                m_counter = 0;
                m_dtSum = 0;
                m_gpuSum = 0;
                m_gpuWaitSum = 0;
            }

            core::uint m_counter;
            float m_dtSum;  
            float m_gpuSum;
            float m_gpuWaitSum;
        };

        Time                                    m_time;
        TimeInternal                            m_timeInternal;
	};
}

#if VG_ENABLE_INLINE
#include "Engine.inl"
#endif