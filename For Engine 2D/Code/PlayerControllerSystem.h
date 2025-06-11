#pragma once
#include "ISystemBase.h"

#include "Window.h"

namespace FE2D {
	class FOR_API PlayerControllerSystem : public ISystemBase {
	public:
		PlayerControllerSystem() = default;
		~PlayerControllerSystem() { this->Release(); }

		void Release();
		void Initialize();

		json Serialize()const override;
		void Deserialize(const json& j)override;

		void Update();
		void OnPropertiesWindow();

	private:
		Entity m_Player;
	};
}