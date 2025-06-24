#pragma once
#include "Entity.h"

namespace FE2D {
	struct FOR_API EntityField {
		Entity* operator->() { return &get(); }
		const Entity* operator->()const { return &get(); }

		Entity& operator*() { return get(); }
		const Entity& operator*()const { return get(); }

		operator Entity& () { return get(); }
		operator const Entity& ()const { return get(); }

		Entity operator=(Entity e) {
			entity = e;
			return entity;
		}

		Entity& get() {
#ifdef _DEBUG  
			check_valid();
#endif  
			return entity;
		}

		const Entity& get()const {
#ifdef _DEBUG  
			check_valid();
#endif  
			return entity;
		}

	private:
		Entity entity;

#ifdef _DEBUG  
		void check_valid()const {
			if (!entity) FOR_RUNTIME_ERROR("Entity wasn't initialized");
		}
#endif

		friend class SceneSerializer;
	};
}
