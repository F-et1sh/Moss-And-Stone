#include "forpch.h"
#include "Entity.h"

FE2D::Entity::Entity(entt::entity handle, Scene* scene) :
	m_EntityHandle(handle), m_Scene(scene) {}

void FE2D::Entity::SetParent(Entity newParent) {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

	auto children = this->GetChildren();
	if (std::find(children.begin(), children.end(), newParent) != children.end())
		return;

	auto& relationship = GetComponent<RelationshipComponent>();

	if (HasParent()) {
		auto& oldParentRel = GetParent().GetComponent<RelationshipComponent>();
		oldParentRel.children.erase(
			std::remove(oldParentRel.children.begin(), oldParentRel.children.end(), GetUUID()),
			oldParentRel.children.end()
		);
	}

	relationship.parent = newParent.GetComponent<IDComponent>().id;

	newParent.GetComponent<RelationshipComponent>().children.emplace_back(GetUUID());
}

Entity FE2D::Entity::GetParent() {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
	return m_Scene->GetEntityByUUID(GetComponent<RelationshipComponent>().parent.get());
}

bool FE2D::Entity::HasParent() {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
	return GetComponent<RelationshipComponent>().parent != 0;
}

void FE2D::Entity::RemoveParent() {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
	FOR_ASSERT(HasParent(), "Entity does not have parent");

	auto& relationship = GetComponent<RelationshipComponent>();

	Entity parentEntity = GetParent();
	auto& parentRel = parentEntity.GetComponent<RelationshipComponent>();
	parentRel.children.erase(
		std::remove(parentRel.children.begin(), parentRel.children.end(), GetUUID()),
		parentRel.children.end()
	);

	relationship.parent = 0;
}

void FE2D::Entity::AddChild() {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

	Entity child = this->m_Scene->CreateEntity("New Child");
	child.SetParent(*this);
}

void FE2D::Entity::AddChild(Entity child) {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
	child.SetParent(*this);
}

std::vector<Entity> FE2D::Entity::GetChildren() {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

	const auto& source = GetComponent<RelationshipComponent>().children;
	std::vector<Entity> children;
	children.reserve(source.size());

	for (auto& child : source)
		children.push_back(m_Scene->GetEntityByUUID(child));

	return children;
}

void FE2D::Entity::RemoveChild(UUID child) {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

	auto& children = GetComponent<RelationshipComponent>().children;
	auto it = std::find(children.begin(), children.end(), child);

	children.erase(it);

	m_Scene->GetEntityByUUID(child).RemoveParent();
}

inline entt::registry& FE2D::Entity::GetRegistry() const noexcept { return m_Scene->m_Registry; }
