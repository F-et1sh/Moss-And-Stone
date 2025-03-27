#pragma once
#include "Serializable.h"
#include "Actor.h"
#include "RenderManager.h"
#include "Input.h"
#include "UCameraComponent.h"

namespace FE2D {
	using ActorList = std::vector<AActor*>;

	class FOR_API Scene {
	public:
		Scene(size_t scene_id) : m_SceneID(scene_id) {}
		~Scene() = default;
	private:
		size_t m_SceneID = 0;
	public:
		inline size_t getSceneID()const noexcept { return m_SceneID; }
	public:
		bool SerializeToFolder(const std::filesystem::path& scene_folder_path)const;
		bool DeserializeFromFolder(const std::filesystem::path& scene_folder_path);
	public:
		void Release();
		void Initialize(ResourceManager* resourceManager, Window* window);

		inline void setEditorCamera(Camera* camera) {
			if (camera)
				m_Renderer.setCamera(camera);
		}

		void End();
		void Start();
		void Update(double _deltaTime, Input& input);
		void Render();
	
	private:
		Camera m_Camera;

		RenderManager m_Renderer;

	public:
		inline ActorList* getActorList()noexcept { return &m_ActorList; }

	private:
		ActorList m_ActorList;
	};
}