#pragma once

namespace FE2D {
	class FOR_API Rectangle {
	public:
		Rectangle() = default;
		~Rectangle() = default;

	public:
		vec2 m_Position = vec2();

		float m_Layer = 0.0f;

		// Flag for Auto Sprite Sorting
		bool m_AutoSort = false;

		vec2 m_Size = vec2(1.0f, 1.0f);

		float m_Rotation = 0.0f;
		vec2  m_Origin = vec2();

		vec4 m_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}