#include "forpch.h"
#include "Fields.h"

bool FE2D::IField::is_valid() const {
	if (!scene) return false;
	if (uuid == FE2D::UUID(0)) return false;

	auto& entity_map = scene->getEntityMap();
	auto it = entity_map.find(uuid);
	if (it != entity_map.end()) return true;
	else return false;
}

const Entity& FE2D::IField::get()const {
	if (!scene) FOR_RUNTIME_ERROR("There is no context");
	else {
		auto& entity_map = scene->getEntityMap();
		auto it = entity_map.find(uuid);
		if (it != entity_map.end())
			return it->second;
		else FOR_RUNTIME_ERROR("Failed to find entity");
	}
}

Entity& FE2D::IField::get() {
	if (!scene) FOR_RUNTIME_ERROR("There is no context");
	else {
		auto& entity_map = scene->getEntityMap();
		auto it = entity_map.find(uuid);
		if (it != entity_map.end())
			return it->second;
		else FOR_RUNTIME_ERROR("Failed to find entity");
	}
}