#pragma once
#include "ISystemBase.h"
#include "Window.h"

namespace FE2D {
	class FOR_API PlayerControllerSystem : public ISystemBase {
	public:
		enum class ViewDirections {
			NONE,

			UP,
			DOWN,

			RIGHT_UP,
			RIGHT_DOWN,

			LEFT_UP,
			LEFT_DOWN,
		};
		inline static constexpr std::array<std::pair<glm::vec2, ViewDirections>, 6> m_ViewDirections = {
			std::make_pair(vec2( 0.0,  1.00), ViewDirections::UP),
			std::make_pair(vec2( 0.0, -1.00), ViewDirections::DOWN),

			std::make_pair(vec2( 0.5,  0.82), ViewDirections::RIGHT_UP),
			std::make_pair(vec2( 0.5, -0.82), ViewDirections::RIGHT_DOWN),

			std::make_pair(vec2(-0.5,  0.82), ViewDirections::LEFT_UP),
			std::make_pair(vec2(-0.5, -0.82), ViewDirections::LEFT_DOWN),
		};
	public:
		PlayerControllerSystem() = default;
		~PlayerControllerSystem() { this->Release(); }

		void Release();
		void Initialize();

		void OnPropertiesPanel(IMGUI& imgui)override;

		json Serialize()const override;
		void Deserialize(const json& j)override;

		void Update();

	private:
		ViewDirections GetViewDirection();

		template<typename T>
		void CheckComponent() {
			if (m_Player.HasComponent<T>()) return;
			SAY("WARNING : Player hasn't component " << typeid(T).name());
			m_Player = {};
		}

		void CheckAnimation(const CharacterAnimatorComponent& component, const std::string& name) {
			if (component.animations.contains(name)) return;
			SAY("WARNING : Player hasn't animation " << name.c_str());
			m_Player = {};
		}

	private:
		Entity m_Player{};
	};
}