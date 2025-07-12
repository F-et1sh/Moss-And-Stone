#pragma once
#include "ISystemBase.h"

#include "Shader.h"
#include "TextureAtlas.h"

#include "VertexArray.h"
#include "VertexBuffer.h"

#include "Components.h"

namespace FE2D {
	static constexpr size_t FOR_SPRITE_LIMIT = 256; // Limit of Sprites per one drawcall
	static constexpr vec2 FOR_TEXTURE_ATLAS_SIZE = vec2(4096*3, 4096*3); // Initial Size of Texture Atlas. HARDCODED, DO NOT TOUCH IT YET
}

namespace FE2D {
	class MousePicker; // forward declaration

	class FOR_API SpriteRendererSystem : public ISystemBase {
	public:
		SpriteRendererSystem();
		~SpriteRendererSystem() = default;

		void Render();

		void RenderPickable(RenderContext& render_context, MousePicker& mouse_picker);

	private:
		void DrawPickable(RenderContext& render_context, Shader& shader, UniformBuffer& ubo);

	private:
		void Handle(Entity entity);
		void DrawSprites();

	private:
		Shader m_Shader;
		UniformBuffer m_UniformBuffer;

		TextureAtlas m_TextureAtlas;

		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;
	private:
		FE2D::dynamic_array<mat4, FOR_SPRITE_LIMIT> m_Matrices;
		FE2D::dynamic_array<vec4, FOR_SPRITE_LIMIT> m_AtlasOffsets;

	private:
		FE2D::dynamic_array<vec4> m_EntityHandles;
	};
}