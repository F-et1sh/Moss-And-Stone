#include "forpch.h"
#include "Entity.h"

FE2D::Entity::Entity(entt::entity handle, Scene* scene) :
	m_EntityHandle(handle), m_Scene(scene) {}

void FE2D::Entity::SetParent(Entity new_parent) {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");

	auto children = this->GetChildren();
	if (std::find(children.begin(), children.end(), new_parent) != children.end())
		return;

	auto& relationship = GetComponent<RelationshipComponent>();

	if (HasParent()) {
		auto& oldParentRel = GetParent().GetComponent<RelationshipComponent>();
		oldParentRel.children.erase(
			std::remove(oldParentRel.children.begin(), oldParentRel.children.end(), GetUUID()),
			oldParentRel.children.end()
		);
	}

	relationship.parent = new_parent.GetComponent<IDComponent>().id;

	new_parent.GetComponent<RelationshipComponent>().children.emplace_back(GetUUID());
}

Entity FE2D::Entity::GetParent() {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
	return m_Scene->GetEntityByUUID(GetComponent<RelationshipComponent>().parent.get());
}

bool FE2D::Entity::HasParent() {
	FOR_ASSERT(HasComponent<RelationshipComponent>(), "Entity does not have RelationshipComponent");
	return GetComponent<RelationshipComponent>().parent != FE2D::UUID(0);
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

	relationship.parent = FE2D::UUID(0);
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

inline FE2D::UUID FE2D::Entity::GetUUID() const {
	FOR_ASSERT(HasComponent<IDComponent>(), "Entity does not have IDComponent");
	return GetComponent<IDComponent>().id;
}

inline mat4 FE2D::Entity::GetGlobalTransform() {
	auto& transform = GetComponent<TransformComponent>();
	mat4 global_transform = mat4();
	if (HasParent())
		global_transform = GetGlobalTransform(GetParent()) * static_cast<mat4>(transform);
	else
		global_transform = static_cast<mat4>(transform);
	
	return global_transform;
}

inline mat4 FE2D::Entity::GetGlobalTransform(Entity entity) {
	auto& transform = entity.GetComponent<TransformComponent>();
	mat4 global_transform = mat4();
	if (entity.HasParent())
		global_transform = GetGlobalTransform(entity.GetParent()) * static_cast<mat4>(transform);
	else
		global_transform = static_cast<mat4>(transform);
	return global_transform;
}

inline entt::registry& FE2D::Entity::GetRegistry() const noexcept { return m_Scene->m_Registry; }

void Entity::BindEntities(Entity& parent, Entity& child) {
	FOR_ASSERT(parent.HasComponent<RelationshipComponent>(), "Parent does not have RelationshipComponent");
	FOR_ASSERT(child.HasComponent<RelationshipComponent>(), "Child does not have RelationshipComponent");
	FOR_ASSERT(parent != child, "Parent and child are the same entity");
	FOR_ASSERT(parent.m_Scene == child.m_Scene, "Parent and child from different scenes");

	Scene* scene = parent.m_Scene;

	auto& parent_relationship = parent.GetComponent<RelationshipComponent>();
	auto& child_relationship = child.GetComponent<RelationshipComponent>();

	if (child_relationship.parent != FE2D::UUID(0)) {
		FE2D::UUID old_parent_uuid = child_relationship.parent;

		if (old_parent_uuid == parent.GetUUID()) return;

		auto it = scene->getEntityMap().find(old_parent_uuid);
		if (it != scene->getEntityMap().end() && it->second.HasComponent<RelationshipComponent>()) {
			auto& old_parent_relationship = it->second.GetComponent<RelationshipComponent>();
			std::erase(old_parent_relationship.children, child.GetUUID());
		}
	}

	FE2D::UUID current_uuid = parent.GetUUID();
	while (true) {
		const auto& current_relationship = scene->getEntityMap().at(current_uuid).GetComponent<RelationshipComponent>();
		if (current_relationship.parent == FE2D::UUID(0)) break;

		if (current_relationship.parent == child.GetUUID()) FOR_ASSERT(false, "Binding would create circular relationship");

		current_uuid = current_relationship.parent;
	}

	if (std::find(parent_relationship.children.begin(), parent_relationship.children.end(), child.GetUUID()) == parent_relationship.children.end())
		parent_relationship.children.emplace_back(child.GetUUID());
	child_relationship.parent = parent.GetUUID();
}

void Entity::UnbindEntities(Entity& parent, Entity& child) {
	FOR_ASSERT(parent.HasComponent<RelationshipComponent>(), "Parent does not have RelationshipComponent");
	FOR_ASSERT(child.HasComponent<RelationshipComponent>(), "Child does not have RelationshipComponent");
	FOR_ASSERT(parent != child, "Parent and child are the same entity");
	FOR_ASSERT(parent.m_Scene == child.m_Scene, "Parent and child from different scenes");

	Scene* scene = parent.m_Scene;

	auto& parent_relationship = parent.GetComponent<RelationshipComponent>();
	auto& child_relationship = child.GetComponent<RelationshipComponent>();

	if (child_relationship.parent != parent.GetUUID()) return;

	std::erase(parent_relationship.children, child.GetUUID());
	child_relationship.parent = FE2D::UUID(0);
}
