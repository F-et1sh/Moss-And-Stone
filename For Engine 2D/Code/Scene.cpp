#include "forpch.h"
#include "Scene.h"

void FE2D::Scene::Release() {
	for (auto& system : m_Systems) {
		system->Release();
	}
}

void FE2D::Scene::Initialize(Window* window, ResourceManager* resourceManager) {

	m_Systems.emplace_back(std::make_unique<SpriteRendererSystem>());
	// add here
	// ..

	for (auto& system : m_Systems) {
		system->setContext(window, resourceManager, this);
		system->Initialize();
	}
}

Entity FE2D::Scene::CreateEntity(const std::string& name) {
	Entity entity = { m_Registry.create(), this };
	entity.AddComponent<TransformComponent>();
	entity.AddComponent<TagComponent>(name.empty() ? "New Entity" : name);
	entity.AddComponent<RelationshipComponent>();
	return entity;
}

void FE2D::Scene::DestroyEntity(Entity entity) {
	m_Registry.destroy(entity);
}

void FE2D::Scene::StartGame() {
	
}

void FE2D::Scene::EndGame()
{
}

void FE2D::Scene::UpdateGame() {
	for (auto& system : m_Systems) {
		system->Update();
	}
}

void FE2D::Scene::RenderGame() {
	for (auto& system : m_Systems) {
		system->Render();
	}
}
