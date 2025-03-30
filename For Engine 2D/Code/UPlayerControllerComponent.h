#pragma once

namespace FE2D {
	class FOR_API UPlayerControllerComponent : public UComponent {
	public:
		UPlayerControllerComponent() = default;
		~UPlayerControllerComponent() = default;

		UTransformComponent* transform = nullptr;
		double deltaTime = 0.0f;

		bool Initialize()override {
			transform = this->getComponent<UTransformComponent>();

			return true;
		}

		void Update(double dt) {
			deltaTime = dt;
		}

		void OnInput(Input& input)override {
			transform->setPosition(transform->getPosition() + input.getDirection() * 1000.0f * deltaTime);
		}
	};
}