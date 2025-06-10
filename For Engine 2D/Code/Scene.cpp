#include "forpch.h"
#include "Scene.h"

void FE2D::Scene::Release() {
	for (const auto& pair : m_EntityMap) {
		Entity e = pair.second;

		if (e.HasComponent<SpriteComponent>()) {
			m_ResourceManager->RemoveResource(e.GetComponent<SpriteComponent>().texture);
		}
		if (e.HasComponent<CharacterAnimatorComponent>()) {
			const auto& animations = e.GetComponent<CharacterAnimatorComponent>().animations;
			for (const auto& pair : animations) {
				m_ResourceManager->RemoveResource(pair.second);
			}
		}
	}

	m_EntityMap.clear();
	m_Registry.clear();
}

void FE2D::Scene::Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager) {
	m_Window = &window;
	m_RenderContext = &render_context;
	m_ResourceManager = &resource_manager;

	m_SpriteRendererSystem	 = this->CreateSystem<SpriteRendererSystem>();
	m_PlayerControllerSystem = this->CreateSystem<PlayerControllerSystem>();
	m_PhysicsSystem			 = this->CreateSystem<PhysicsSystem>();
	m_AnimationSystem		 = this->CreateSystem<AnimationSystem>();
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

Entity FE2D::Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
	Entity entity = { m_Registry.create(), this };
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TagComponent>(name.empty() ? "New Entity" : name);
	entity.AddComponent<TransformComponent>();
	entity.AddComponent<RelationshipComponent>();

	m_EntityMap.emplace(uuid, entity);

	return entity;
}

void FE2D::Scene::DestroyEntity(Entity entity) {
	if (entity.HasParent())
		entity.GetParent().RemoveChild(entity.GetUUID());

	for (auto& child : entity.GetChildren()) {
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

	/* Input */
	m_PlayerControllerSystem->Update();
	
	/* Calculations */
	m_PhysicsSystem->Update();
	
	/* Post-Calculations */
	// ..

}

void FE2D::Scene::Render() {
	
	/* Animation */
	m_AnimationSystem->Render();

	/* Sprite Renderer */
	m_SpriteRendererSystem->Render();

	/* .. */
	//..
}

void FE2D::Scene::OnSystemPropertiesWindow() {
	m_PlayerControllerSystem->OnPropertiesWindow();
	// ..
}

void FE2D::Scene::RenderPickable(RenderContext& render_context, MousePicker& mouse_picker) {
	m_SpriteRendererSystem->RenderPickable(render_context, mouse_picker);
	// ..
}
