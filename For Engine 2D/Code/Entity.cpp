#include "forpch.h"
#include "Entity.h"

FE2D::Entity::Entity(entt::entity handle, Scene* scene) :
	m_EntityHandle(handle), m_Scene(scene) {}