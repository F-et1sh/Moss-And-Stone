#pragma once
#include "Entity.h"

namespace FE2D {  
	template<typename T>  
	struct FOR_API ComponentField {
		T* operator->() { return &get(); }  
		const T* operator->()const { return &get(); }

		T& operator*() { return get(); }  
		const T& operator*()const { return get(); }  

		operator T& () { return get(); }  
		operator const T& ()const { return get(); }

		Entity operator=(Entity e) {
			entity = e;
			return entity;
		}

		T& get() {  
#ifdef _DEBUG  
			check_valid();
#endif  
			return entity.GetComponent<T>();
		}  

		const T& get()const {  
#ifdef _DEBUG  
			check_valid();
#endif  
			return entity.GetComponent<T>();
		}  

	private:
		Entity entity;

#ifdef _DEBUG  
		void check_valid()const {  
			if (!entity) this->assert_error("Entity wasn't initialized");  
			if (!entity.HasComponent<T>()) this->assert_error("Entity hasn't this component");  
		}  

        void assert_error(const std::string& message) const {  
            std::string entity_name = "<unknown>";  
            static const std::string component_name = typeid(T).name();  

            if (entity && entity.HasComponent<TagComponent>())  
                entity_name = entity.GetComponent<TagComponent>().tag;  

            std::ostringstream error_stream;  
            error_stream << "ComponentField failure\nEntity name : " << entity_name  
                         << "\nComponent name : " << component_name  
                         << "\nMessage : " << message;  

			FOR_RUNTIME_ERROR(error_stream.str());
        }  
#endif

		friend class SceneSerializer;
	};  
}
