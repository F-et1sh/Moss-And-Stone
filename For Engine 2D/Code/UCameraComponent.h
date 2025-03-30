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

		const vec2& getVisionSize()const noexcept { return m_VisionSize; }

	private:
		vec2 m_VisionSize = vec2();
	};
}