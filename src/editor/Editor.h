#pragma once

#include "editor/IEditor.h"
#include "core/Singleton/Singleton.h"

namespace vg
{
	namespace core
	{
		class IFactory;
		class ISelection;
		class IBaseScene;
	}

	namespace engine
	{
		class IEngine;
	}

	namespace renderer
	{
		class IRenderer;
	}

	namespace editor
	{
		class ImGuiWindow;

		class Editor final : public IEditor, public core::Singleton<Editor>
		{
		public:
			using super = IEditor;

			IPlugin::Version				    GetVersion			() const final override;

												Editor				(const core::string & _name, core::IObject * _parent);
												~Editor				();

			bool								RegisterClasses		() final override;
			bool								UnregisterClasses	() final override;

			#ifdef _WIN32
			LRESULT CALLBACK                    WndProc				(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final override;
			#endif

			void							    Init				(const core::Singletons & _singletons) final override;
			void							    Deinit				() final override;

			void								RunOneFrame			(float _dt) final override;

			void							    DrawGUI				(const GUIContext & _context) final override;

			IEditorOptions *					GetOptions			() const final override;

			template <class T> T *				getWindow			(const core::string _name = {}) const;
			template <class T> core::vector<T*>	getWindows			(const core::string _name = {}) const;
			void								destroyWindow		(ImGuiWindow * _window);

			core::IFactory *					getFactory			() const;
			core::ISelection *					getSelection		() const;
            engine::IEngine *					getEngine			() const;
            renderer::IRenderer *				getRenderer			() const;

			void								openPrefabView		(const core::IBaseScene * _prefab);

		//private:
			core::vector<ImGuiWindow *>			m_imGuiWindows;
			core::vector<ImGuiWindow *>			m_imGuiWindowsToDestroy;
		};
	}
}

#if VG_ENABLE_INLINE
#include "Editor.inl"
#endif