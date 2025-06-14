#pragma once
#include "Entity.h"

namespace FE2D {
	template<typename T>
	struct FOR_API ComponentField {
		Entity entity;
		
		T& get() {
			if (!entity)				   FOR_RUNTIME_ERROR(std::string("Failed to get component\nEntity wasn't initialized") + "\nComponent : " + typeid(T).name());
			if (!entity.HasComponent<T>()) FOR_RUNTIME_ERROR(("Failed to get component\nEntity hasn't this component\nEntity name : " + entity.GetComponent<TagComponent>().tag) + "\nComponent : " + typeid(T).name());
			return entity.GetComponent<T>();
		}
		const T& get()const {
			if (!entity)				   FOR_RUNTIME_ERROR(std::string("Failed to get component\nEntity wasn't initialized") + "\nComponent : " + typeid(T).name());
			if (!entity.HasComponent<T>()) FOR_RUNTIME_ERROR(("Failed to get component\nEntity hasn't this component\nEntity name : " + entity.GetComponent<TagComponent>().tag) + "\nComponent : " + typeid(T).name());
			return entity.GetComponent<T>();
		}
	};
}