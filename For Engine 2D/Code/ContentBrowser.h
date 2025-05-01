#pragma once
#include "SceneHierarchyPanel.h"

#include "Window.h"
#include "Camera.h"
#include "ButtonManager.h"

#include "Texture.h"

namespace FE2D {
	constexpr ImVec4 DEFAULT_FILE_COLOR = ImVec4(1, 1, 1, 1);
	constexpr ImVec4 DEFAULT_DIRECTORY_COLOR = ImVec4(0, 0, 0, 0);

	constexpr ImVec4 SELECTED_FILE_COLOR = ImVec4(0, 0.5, 0.5, 1);
	constexpr ImVec4 SELECTED_DIRECTORY_COLOR = ImVec4(0, 1, 1, 0.5);

	class FOR_API ContentBrowser {
	public:
		ContentBrowser() = default;
		~ContentBrowser() = default;
	public:
		void Release();
		void Initialize(Window& window, ResourceManager& resource_manager, IMGUI& imgui, SceneHierarchyPanel& scene_hierarchy_panel);

		void OnDeleteRequest();
		void OnImGuiRender();
	private:
		void OnEditorDraw();
		void OnPanelDraw();
	private:
		void DrawDirectory(const std::filesystem::path& path);
		void DrawFile(const std::filesystem::path& path);
	private:
		void LoadContentImage(Texture& texture, const std::filesystem::path& path);
	private:
		inline IResource* getSelected()const noexcept {
			return m_Selected;
		}
		inline void setSelected(IResource* resource, const std::filesystem::path& path)noexcept {
			m_SceneHierarchyPanel->resetSelected();
			m_Selected = resource;
			m_SelectedPath = path;
		}
		inline void resetSelected()noexcept { 
			m_Selected = nullptr;
			m_SelectedPath = "";
		}
	private:
		inline const std::filesystem::path& getDeletingPath()const noexcept { 
			return m_DeletingPath; 
		}
		inline void setDeletingPath(const std::filesystem::path& path)noexcept { 
			m_DeletingPath = path; 
			m_DeleteRequest = true;
		}
		inline void resetDeletingPath()noexcept { 
			m_DeletingPath = ""; 
			m_DeleteRequest = false; 
		}
	private:
		Texture m_DirectoryImage;
		Texture m_EmptyImage;
		Texture m_AnimationImage;
	private:
		std::filesystem::path m_RootDirectory;    // path to the assets folder
		std::filesystem::path m_CurrentDirectory; // current opened directory

		float m_ItemSize = 160.0f; // size of files and folders in the UI window
	private:
		Window* m_Window = nullptr;
		size_t m_EventIndex = 0; // to unsubscribe the event
	private:
		ResourceManager* m_ResourceManager = nullptr;
	private:
		IResource* m_Selected = nullptr;
		std::filesystem::path m_SelectedPath;
	private:
		std::filesystem::path m_DeletingPath;
	public:
		bool m_DeleteRequest = false;
	private:
		IMGUI* m_ImGui = nullptr;
		SceneHierarchyPanel* m_SceneHierarchyPanel = nullptr;

	private:
		friend class FOR_API SceneHierarchyPanel;
	};
}