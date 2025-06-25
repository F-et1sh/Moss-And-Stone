#pragma once

namespace FE2D {
	struct FOR_API IField {
		IField() = default;
		virtual ~IField() = default;

		void init(Scene& scene);

		const Entity& get()const {
#ifdef _DEBUG
			this->check_valid();
#endif
			return entity;
		}

		Entity& get() {
#ifdef _DEBUG
			this->check_valid();
#endif
			return entity;
		}


	protected:
		Entity entity;
		FE2D::UUID uuid;

#ifdef _DEBUG
		virtual void check_valid()const {
			if (!entity) FOR_RUNTIME_ERROR("Field entity wasn't initialized");
		}
#endif

		friend class IMGUI;
		friend class SceneSerializer;
		friend class ScirptableEntity;
	};

	template<typename T>
	struct FOR_API ComponentField : public IField {
		ComponentField() = default;
		~ComponentField() = default;

		Entity operator=(Entity e) {
			this->entity = e;
			return this->entity;
		}

		T* operator->() { return &get_component(); }
		const T* operator->()const { return &get_component(); }

	private:
		T& get_component() {
			Entity e = this->get();
			return e.GetComponent<T>();
		}

		const T& get_component()const {
			Entity e = this->get();
			return e.GetComponent<T>();
		}

#ifdef _DEBUG
		void check_valid()const override {
			IField::check_valid();
			if (!entity.HasComponent<T>()) FOR_RUNTIME_ERROR("Field entity wasn't needed component");
		}
#endif
	};

	struct FOR_API EntityField : public IField {
		EntityField() = default;
		~EntityField() = default;

		Entity operator=(Entity e) {
			this->entity = e;
			return this->entity;
		}

		Entity* operator->() { return &get(); }
		const Entity* operator->()const { return &get(); }
	};
}