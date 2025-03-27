#pragma once
#include "Shader.h"
#include "Camera.h"

#include "Rectangle.h"

#include "VertexArray.h"
#include "VertexBuffer.h"

namespace FE2D {
	class FOR_API RectangleRenderer {
	public:
		RectangleRenderer() = default;
		~RectangleRenderer() = default;

		static RectangleRenderer& Instance() {
			static RectangleRenderer instance;
			return instance;
		}

		void Release();
		const bool Initialize();

		inline void RegisterRectangle(const Rectangle& load_rectangle);
		void RenderView();

	private:
		Shader m_Shader;
	private:
		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;
	};
}