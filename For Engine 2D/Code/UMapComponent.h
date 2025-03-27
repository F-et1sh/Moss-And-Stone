#pragma once
#include "Actor.h"

namespace FE2D {
	class FOR_API UMapComponent : public UComponent {
	public:
		UMapComponent() = default;
		~UMapComponent() = default;

		bool Initialize()override {

			AActor* children = new AActor();
			children->getComponent<UTransformComponent>()->setPosition(vec2(500, 0));
			
			children->addComponent<USpriteComponent>()->m_TextureIndex = m_Textures[0];
			children->getComponent<USpriteComponent>()->m_TextureCoords = vec4(0,0,FOR_IMGUI.getResourceManager()->getResource<Texture>(m_Textures[0])->getSize());

			this->getActor()->addChildren(children);

			return true;
		}

		void show_properties()override {
			if (ImGui::Button("Add Texture")) {
				m_Textures.push_back(FOR_IMGUI.getResourceManager()->m_ResourceCache.get_resource_array().at(typeid(Texture).hash_code()).begin()->first);
			}

			if (m_Textures.empty())
				return;

			auto texture = FOR_IMGUI.getResourceManager()->getResource<Texture>(*m_Textures.begin());

			ImGui::Image(texture->reference(), { texture->getSize().x,texture->getSize().y }, ImVec2(0, 1), ImVec2(1, 0));
		}

	private:
		std::vector<size_t> m_Textures;
	};
}