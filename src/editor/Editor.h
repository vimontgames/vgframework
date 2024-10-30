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
		class IGameObject;
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
		class ImGuiMenu;

		class Editor final : public IEditor, public core::Singleton<Editor>
		{
		public:
			using super = IEditor;

            Editor(const core::string & _name, core::IObject * _parent);
            ~Editor();

			bool								RegisterClasses		() final override;
			bool								UnregisterClasses	() final override;

			#ifdef _WIN32
			LRESULT CALLBACK                    WndProc				(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final override;
			#endif

			void							    Init				(const core::Singletons & _singletons) final override;
			void							    Deinit				() final override;

			void								RunOneFrame			() final override;

			void							    DrawGUI				(const GUIContext & _context) final override;

			IEditorOptions *					GetOptions			() const final override;

			const core::vector<ImGuiWindow *> & GetWindows			() const { return m_imGuiWindows; }
			template <class T> T *				getMenu				(const core::string _name = {}) const;

			core::IFactory *					getFactory			() const;
			core::ISelection *					getSelection		() const;

			engine::IEngine *					findEngine			() const;
			renderer::IRenderer *				findRenderer		() const;

            VG_INLINE engine::IEngine *			getEngine			() const { return m_engine; }
            VG_INLINE renderer::IRenderer *		getRenderer			() const { return m_renderer; }

			void								openPrefabView		(const core::IResource * _prefabRes);
			void								focus				(core::IGameObject * _gameObject);
			void								focus				(const core::vector<core::IGameObject *> & _gameObjects);
			void								deleteGameObjects	(core::vector<core::IGameObject*> & _gameObjects);

			void								destroyWindow		(ImGuiWindow * _window);

		private:
			template <class T> T *				getWindow			(const core::string _name = {}) const;
			template <class T> core::vector<T*>	getWindows			(const core::string _name = {}) const;

		private:
			core::vector<ImGuiWindow *>			m_imGuiWindows;
			core::vector<ImGuiWindow *>			m_imGuiWindowsToDestroy;
			core::vector<ImGuiMenu *>			m_imGuiMenus;

			vg::engine::IEngine *				m_engine			= nullptr;
			vg::renderer::IRenderer *			m_renderer			= nullptr;
		};

		//--------------------------------------------------------------------------------------
		template <class T> T * Editor::getMenu(const core::string _name) const
		{
			for (auto i = 0; i < m_imGuiMenus.size(); ++i)
			{
				if (dynamic_cast<T *>(m_imGuiMenus[i]) != nullptr && (_name.empty() || ((Object *)m_imGuiWindows[i])->GetName()._Starts_with(_name)))
					return (T *)(m_imGuiMenus[i]);
			}
			return nullptr;
		}
	}
}

#if VG_ENABLE_INLINE
#include "Editor.inl"
#endif