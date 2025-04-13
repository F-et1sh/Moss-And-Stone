#pragma once
#include "Scene.h"

namespace FE2D {
	using SceneIndex = size_t;
	
	constexpr SceneIndex START_SCENE = 0;
		  
	class FOR_API SceneManager {
	public:
		SceneManager() = default;
		~SceneManager() = default;

		void Release();
		void Initialize(Window* window, ResourceManager* resourceManager);

		void Update();

		inline void RenderPickable(Shader& shader, UniformBuffer& ubo);
		
		void SaveCurrentScene();

	public:
		inline void setCamera(Camera* editor_camera)noexcept { return m_CurrentScene.setCamera(editor_camera); }
		inline Camera& getCamera()noexcept { return m_CurrentScene.getCamera(); }
		
	public:
		inline void StartGameSession()noexcept { m_IsRunning = true; m_CurrentScene.setCamera(nullptr); }
		inline void EndGameSession()noexcept { m_IsRunning = false; }

	private:
		bool m_IsRunning = false;

	private:
		void LoadAvailableScenes();
		bool LoadScene(SceneIndex index);

	private:
		std::unordered_map<SceneIndex, std::filesystem::path> m_ScenePaths;

	private:
		Window* m_Window = nullptr;
		ResourceManager* m_ResourceManager = nullptr;

	public:
		// get a reference of current scene
		inline Scene& getCurrentScene()noexcept { return m_CurrentScene; }

	private:
		Scene m_CurrentScene;
	};
}