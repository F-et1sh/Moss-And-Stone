#include "forpch.h"
#include "Scene.h"

void FE2D::Scene::Release() {
	for (auto& system : m_Systems) {
		system->Release();
	}
	m_EntityMap.clear();
	m_Registry.clear();
}

void FE2D::Scene::Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager) {

	m_Systems.emplace_back(std::make_unique<SpriteRendererSystem>());
	// add here
	// ..

	for (auto& system : m_Systems) {
		system->setContext(window, render_context, resource_manager, *this);
		system->Initialize();
	}

	m_RenderContext = &render_context;
}

Entity FE2D::Scene::CreateEntity(const std::string& name) {
	Entity entity = { m_Registry.create(), this };
	UUID uuid = entity.AddComponent<IDComponent>().id;
	entity.AddComponent<TagComponent>(name.empty() ? "New Entity" : name);
	entity.AddComponent<TransformComponent>();
	entity.AddComponent<RelationshipComponent>();

	m_EntityMap.emplace(uuid, entity);

	return entity;
}

void FE2D::Scene::DestroyEntity(Entity entity) {
	if (entity.HasParent())
		entity.GetParent().RemoveChild(entity.GetUUID());

	for (auto child : entity.GetChildren()) {
		entity.RemoveChild(child.GetUUID());

		m_EntityMap.erase(child.GetUUID());
		m_Registry.destroy(child);
	}

	m_EntityMap.erase(entity.GetUUID());
	m_Registry.destroy(entity);
}

void FE2D::Scene::EmplaceEntity(Entity entity) {
	m_EntityMap.emplace(entity.GetUUID(), entity);
}

void FE2D::Scene::End()
{
}

void FE2D::Scene::Start() {
	bool camera_found = false;

	auto view = m_Registry.view<TransformComponent, CameraComponent>();
	view.each([&](auto entity, auto& transform, auto& camera) {
		m_Camera.setPosition(transform.position);

		camera_found = true;
		});

	if (!camera_found)
		SAY("WARNING : No camera found in the Scene");

	m_RenderContext->BindCamera(m_Camera);
}

void FE2D::Scene::Update() {
	for (auto& system : m_Systems) {
		system->Update();
	}
}

void FE2D::Scene::Render() {
	for (auto& system : m_Systems) {
		system->Render();
	}
}

void FE2D::Scene::RenderPickable(RenderContext& render_context, MousePicker& mouse_picker) {
	for (auto& system : m_Systems) {
		system->RenderPickable(render_context, mouse_picker);
	}
}
