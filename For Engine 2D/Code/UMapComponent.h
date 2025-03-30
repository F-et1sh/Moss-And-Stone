#pragma once
#include "Actor.h"

namespace FE2D {
	class FOR_API UMapComponent : public UComponent {
	public:
		UMapComponent() = default;
		~UMapComponent() = default;

		bool Initialize()override {

			AActor* children = new AActor();

			children->addComponent<USpriteComponent>()->m_TextureIndex = m_Texture;
			children->getComponent<USpriteComponent>()->m_TextureCoords = vec4(0, 0, m_Size);

			this->getActor()->addChildren(children);

			return true;
		}

		void show_properties()override {
			FOR_IMGUI.SelectTexture("Texture", m_Texture);
			FOR_IMGUI.DragVector2("Size", m_Size);
		}

	private:
		size_t m_Texture = 0;

		vec2 m_Size = vec2();
	};
}