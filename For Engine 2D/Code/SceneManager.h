#pragma once
#include "Scene.h"

namespace FE2D {
	class FOR_API SceneManager {
	public:
		SceneManager() = default;
		~SceneManager() = default;
	public:
		void Release();
		void Initialize(Window* window, Input* input, ResourceManager* resource_manager);
		
		inline void setEditorCamera(Camera* camera)noexcept {
			m_EditorCamera = camera;
			this->m_CurrentScene.setEditorCamera(m_EditorCamera);
		}

		void Update();
		void Render();

	public:
		inline void StartGameSession() { this->m_CurrentScene.Start(); }
		inline void StopGameSession() { this->m_CurrentScene.End(); }

	private:
		void FindScenesInFolder();
		void FindDefaultScene(size_t& default_scene_id);

	public:
		void LoadScene(size_t scene_id);

		inline size_t LastSceneID()const noexcept { return m_LastSceneID; }
	public:
		void SerializeCurrentScene();
		void DeserializeScene(size_t scene_id);

		void TMP_SerializeCurrentScene();
		void TMP_DeserializeScene(size_t scene_id);

	private:

		void Remove_TMP_Scenes();

	public:
		inline ActorList* getActorList()noexcept { return m_CurrentScene.getActorList(); }
	private:
		ResourceManager* m_ResourceManager = nullptr;
		
		Window* m_Window = nullptr;
		Input* m_Input = nullptr;

		Camera* m_EditorCamera = nullptr;
	private:
		Scene m_CurrentScene = Scene(0); // set the default scene ID as zero
		size_t m_LastSceneID = 0;
	private:
		// size_t - scene index
		// std::wstring - scene name
		std::unordered_map<size_t, std::wstring> m_SceneCatalog;
	};
}