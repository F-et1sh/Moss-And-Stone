#pragma once
#include "UTransformComponent.h"
#include "USpriteComponent.h"

namespace FE2D {
	class FOR_API Renderer {
	public:
		Renderer() = default;
		~Renderer() = default;

		virtual void Release() { }
		virtual void Initialize(ResourceManager* resourceManager) { }
		virtual void setCamera(Camera* camera) { }
		virtual void Handle(UTransformComponent* transform, const UComponent* component) { }
		// if the Renderer making batch you can use this function.
		// in the Handle() you saving object's info and then here drawing it
		virtual void Render() { }

	private:
		
	};
#define FOR_RENDERER_REGISTER(renderer_type, component_type, name) static FE2D::RendererRegistrar<renderer_type, component_type> type##_register(name);
}