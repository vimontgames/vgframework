#pragma once

#include "editor/IEditor.h"
#include "core/Singleton/Singleton.h"

namespace vg
{
	namespace core
	{
		class IFactory;
		class ISelection;
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

		class Editor : public IEditor, public core::Singleton<Editor>
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

			void							    DrawGUI				(const GUIContext & _context) final override;

			template <class T> T *				getWindow			() const;

			core::IFactory *					getFactory			() const;
			core::ISelection *					getSelection		() const;
            engine::IEngine *					getEngine			() const;
            renderer::IRenderer *				getRenderer			() const;

		private:
			core::vector<ImGuiWindow *>			m_imGuiWindows;
		};
	}
}

#if VG_ENABLE_INLINE
#include "Editor.inl"
#endif