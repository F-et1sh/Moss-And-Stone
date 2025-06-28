#pragma once

namespace FE2D {
	struct FOR_API IField {
		IField() = default;
		virtual ~IField() = default;

		void set_context(Scene* scene)noexcept { this->scene = scene; }
		
		bool is_valid()const;

		const Entity& get()const;
		Entity& get();

	protected:
		Scene* scene = nullptr;
		FE2D::UUID uuid = FE2D::UUID(0);

		friend class IMGUI;
		friend class SceneSerializer;
		friend class ScirptableEntity;
	};

	template<typename T>
	struct FOR_API ComponentField : public IField {
		ComponentField() = default;
		~ComponentField() = default;
		
		Entity operator=(Entity e) {
			this->uuid = e.GetUUID();
			return this->get();
		}

		T* operator->() { return &get_component(); }
		const T* operator->()const { return &get_component(); }

	private:
		inline T& get_component() {
			Entity e = this->get();
			return e.GetComponent<T>();
		}

		inline const T& get_component()const {
			Entity e = this->get();
			return e.GetComponent<T>();
		}
	};

	struct FOR_API EntityField : public IField {
		EntityField() = default;
		~EntityField() = default;
		
		Entity operator=(Entity e) {
			this->uuid = e.GetUUID();
			return this->get();
		}

		Entity* operator->() { return &get(); }
		const Entity* operator->()const { return &get(); }
	};
}