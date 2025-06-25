#include "forpch.h"
#include "Fields.h"

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