#include "forpch.h"
#include "Scene.h"

FE2D::Scene::~Scene() {
	for (const auto& pair : m_EntityMap) {
		Entity e = pair.second;

		if (e.HasComponent<SpriteComponent>()) {
			m_ResourceManager->RemoveResource(e.GetComponent<SpriteComponent>().texture);
		}
	}
	m_SystemsList.clear();
	m_EntityMap.clear();
	m_Registry.clear();
}

void FE2D::Scene::Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager) {
	m_Window		  = &window;
	m_RenderContext	  = &render_context;
	m_ResourceManager = &resource_manager;

	m_SpriteRendererSystem	 = this->CreateSystem<SpriteRendererSystem>();
	m_PhysicsSystem			 = this->CreateSystem<PhysicsSystem>();
	m_AnimationSystem		 = this->CreateSystem<AnimationSystem>();
	m_ScriptManagerSystem	 = this->CreateSystem<ScriptManagerSystem>();
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

void FE2D::Scene::End() {
	this->m_ScriptManagerSystem->OnEnd();
}

void FE2D::Scene::Start() {
	bool camera_found = false;

	auto view = m_Registry.view<TransformComponent, CameraComponent>();
	view.each([&](auto entity, auto& transform, auto& camera) {
		m_CameraEntityUUID = Entity{ entity, this }.GetUUID();
		camera_found = true;
		});
	
	if (!camera_found) SAY("WARNING : No camera found in the Scene");

	m_RenderContext->BindCamera(m_Camera);

	/* Start the scripts */
	m_ScriptManagerSystem->OnStart();
}

void FE2D::Scene::Update() {

	m_ScriptManagerSystem->OnUpdate();

	m_PhysicsSystem->Update();

	if (m_CameraEntityUUID != FE2D::UUID(0)) {
		vec2 cam_pos = this->GetEntityByUUID(m_CameraEntityUUID).GetComponent<TransformComponent>().position;

		double x_pos = 0.0f;
		double y_pos = 0.0f;

		glfwGetCursorPos(m_Window->reference(), &x_pos, &y_pos);

		y_pos = m_Window->getResolution().y - y_pos;

		x_pos -= m_Window->getResolution().x / 2.0f;
		y_pos -= m_Window->getResolution().y / 2.0f;

		vec2 pos = { x_pos , y_pos };

		pos /= 10;

		cam_pos += pos;

		m_Camera.setPosition(cam_pos);
	}
}

void FE2D::Scene::Render() {

	m_AnimationSystem->Render();
	m_SpriteRendererSystem->Render();
}

void FE2D::Scene::RenderPickable(RenderContext& render_context, MousePicker& mouse_picker) {
	m_SpriteRendererSystem->RenderPickable(render_context, mouse_picker);
	// ..
}
