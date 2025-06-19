#pragma once
#include "Scene.h"
#include "MousePicker.h"

namespace FE2D {
	using SceneIndex = size_t;
	constexpr SceneIndex START_SCENE = 0;
		  
	class FOR_API SceneManager {
	public:
		SceneManager() = default;
		~SceneManager() = default;

		void Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager);

		void Update();

		inline void RenderPickable(RenderContext& render_context, MousePicker& mouse_picker) { m_CurrentScene.RenderPickable(render_context, mouse_picker); }
		
		void SaveCurrentScene();

	public:
		inline void StartGameSession() { 
			m_IsRunning = true; 
			m_CurrentScene.Start();

			m_Window->setCursorIcon(FOR_PATH.get_cursors_path() / "CursorQuad.png");
		}
		inline void EndGameSession() { 
			m_IsRunning = false;
			m_CurrentScene.End();

			m_Window->ResetCursorIcon();
		}
		inline bool IsGameSessionRunning()const noexcept { return m_IsRunning; }

	private:
		bool m_IsRunning = false;

	private:
		void LoadAvailableScenes();
		bool LoadScene(SceneIndex index);

	private:
		std::unordered_map<SceneIndex, std::filesystem::path> m_ScenePaths;

	private:
		RenderContext* m_RenderContext = nullptr;
		Window* m_Window = nullptr;
		ResourceManager* m_ResourceManager = nullptr;

	public:
		// get a reference of current scene
		inline Scene& getCurrentScene()noexcept { return m_CurrentScene; }

	private:
		Scene m_CurrentScene;
	};
}