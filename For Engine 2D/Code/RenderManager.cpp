#include "forpch.h"
#include "RenderManager.h"

#include "Actor.h"

void FE2D::RenderManager::Release() {
	for (auto& renderer : m_Renderers) {
		renderer.second->Release();
		delete renderer.second;
	}
	m_Renderers.clear();
}

void FE2D::RenderManager::Initialize(ResourceManager* resourceManager) {
	if (!resourceManager)
		FOR_RUNTIME_ERROR("Failed to Initialize RendererManager\nResourceManager was nullptr");
	m_ResourceManager = resourceManager;
}

void FE2D::RenderManager::setCamera(Camera* camera) noexcept {
	m_Camera = camera;
	for (auto& renderer : m_Renderers) {
		renderer.second->setCamera(m_Camera);
	}
}

void FE2D::RenderManager::HandleActor(AActor* actor) {
	UTransformComponent* transform = actor->getComponent<UTransformComponent>();

	for (const auto& actor_component : actor->getComponents()) {

		size_t hash_code = actor_component.first;
		UComponent* component = actor_component.second.get();

		this->HandleComponent(transform, component, hash_code);
	}

	for (AActor* children : actor->getChildrens())
		this->HandleActor(children);
}

void FE2D::RenderManager::HandleComponent(UTransformComponent* transform, const UComponent* component, size_t hash_code) {
	if (m_Renderers.find(hash_code) == m_Renderers.end()) {

		Renderer* renderer = RendererFactory::Instance().CreateRenderer(hash_code);
		if (!renderer) {
			// if failed to create a renderer it means that the component doesn't have to be renderer
			// or user just forgot to register the renderer
			return;
		}
		renderer->Initialize(m_ResourceManager);
		renderer->setCamera(m_Camera);

		m_Renderers.emplace(hash_code, renderer);
	}
	auto it = m_Renderers.find(hash_code);
	if (it == m_Renderers.end())
		SAY("ERROR : Failed to handle a component\nComponent type : " << typeid(*component).name());

	it->second->Handle(transform, component);
}

void FE2D::RenderManager::Render() {
	for (auto& renderer : m_Renderers) {
		renderer.second->Render();
	}
}
