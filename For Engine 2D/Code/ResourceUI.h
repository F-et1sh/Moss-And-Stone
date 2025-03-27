#pragma once
#include "Window.h"
#include "Camera.h"
#include "ButtonManager.h"

#include "Texture.h"

namespace FE2D {
	class FOR_API InspectorUI;

	// ResourceUI - it's not a module of ResourceManager, but inherits ResorceModule<ResourceUI>
	// It's a window of ResourceManager in FE2D Editor
	// It's can display all of loaded Resources
	class FOR_API ResourceUI : public ResourceModule<ResourceUI> {
	public:
		ResourceUI(ResourceManager* load_resourceManager) {
			setResourceManager(load_resourceManager);
		}
		~ResourceUI() = default;
	public:
		void Release();
		void Initialize(Window* window, InspectorUI* load_inspectorUI);

		void Update();
	private:
		void ShowResources();
		void HandleDeleting();
		void HandleCreating();
		void CancelSelection();
	private:
		Texture m_FolderIconTexture;
		Texture m_FileIconTexture;
	private:
		std::filesystem::path m_FilesDirectory;   // path to the assets folder
		std::filesystem::path m_CurrentDirectory; // current open directory

		float m_ItemSize = 160.0f; // size of files and folders in the UI window
	private:
		InspectorUI* m_InspectorUI = nullptr;
	private:
		Resource* m_Selected = nullptr;
	};
}