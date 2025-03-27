#pragma once
#include "Component.h"
#include "Renderer.h"
#include "Texture.h"

namespace FE2D {
	class FOR_API USpriteComponent : public UComponent {
	public:
		USpriteComponent() = default;
		~USpriteComponent() = default;
	public:
		json Serialize()const override;
		void Deserialize(const json& j)override;
	public:
		size_t m_TextureIndex = 0;

		vec4 m_TextureCoords = vec4(0, 0, 1, 1);
		bool m_FlipX = false;
		bool m_FlipY = false;
	public:

		void show_properties()override;
	};
}