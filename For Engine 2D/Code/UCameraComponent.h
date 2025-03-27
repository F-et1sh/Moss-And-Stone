#pragma once
#include "Component.h"

namespace FE2D {
	class FOR_API UCameraComponent : public UComponent {
	public:
		UCameraComponent() = default;
		~UCameraComponent() = default;

	public:
		json Serialize()const override;
		void Deserialize(const json& j)override;
	public:

		void show_properties()override;

	public:
		vec2 m_VisionSize = vec2();
		vec2 m_Position = vec2();
	};
}