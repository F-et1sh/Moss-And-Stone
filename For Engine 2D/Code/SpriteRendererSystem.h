#pragma once
#include "SystemBase.h"

#include "Shader.h"
#include "TextureAtlas.h"

#include "VertexArray.h"
#include "VertexBuffer.h"

namespace FE2D {
	static constexpr size_t SPRITE_LIMIT = 256; // Limit of Sprites per one drawcall
	static constexpr vec2 TEXTURE_ATLAS_SIZE = vec2(4096, 4096); // Initial Size of Texture Atlas
}

namespace FE2D {
	class FOR_API SpriteRendererSystem : public SystemBase {
	public:
		SpriteRendererSystem() = default;
		~SpriteRendererSystem() = default;

		void Release()override;
		void Initialize()override;

		void setCamera(Camera* camera);
		void Handle(TransformComponent& transform, SpriteComponent& sprite);
		
		void Update()override;
		void Render()override;

	private:
		void DrawSprites();

	private:
		Camera* m_Camera = nullptr;

		Shader m_Shader;
		UniformBuffer m_UniformBuffer;

		TextureAtlas m_TextureAtlas;

		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;
	private:
		FE2D::dynamic_array<mat4, SPRITE_LIMIT> m_Matrices;
		FE2D::dynamic_array<vec4, SPRITE_LIMIT> m_AtlasOffsets;
	};
}