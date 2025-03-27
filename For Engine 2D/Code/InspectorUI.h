#pragma once
#include "ResourceUI.h"

#include "Window.h"
#include "Camera.h"

#include "USpriteComponent.h"

namespace FE2D {
	class FOR_API ActorListUI;
	class FOR_API ISelectable;
	class FOR_API IPropertiesDrawable;

	// InspectorUI is only calling show_properties() function of ISelectable and IPropertiesDrawable
	class FOR_API InspectorUI {
	public:
		InspectorUI() = default;
		~InspectorUI() = default;

	public:
		void Release();
		void Initialize(ResourceUI* load_resourceUI, ActorListUI* load_actorListUI)noexcept;

		void Update();

		// Change selected item, which is displaying on this panel
		// If your argument will be nullptr just nothing will be displayed
		inline void ChangeSelectedItem(ISelectable* item)noexcept {
			m_SelectedItem = item;
		}

	private:
		void actor_name				  (AActor* actor); // draw renamable actor name
		void actor_components		  (AActor* actor); // draw changable all of actor's components
		void actor_AddComponent_button(AActor* actor); // draw Add Component button

	private:
		void resource_name(Resource* resource);  // draw resource's file name
		void resource_path(Resource* resource);  // draw resource's full path

	private:
		ResourceUI* m_ResourceUI = nullptr;
		ActorListUI* m_ActorListUI = nullptr;
	private:
		ISelectable* m_SelectedItem = nullptr;
	};
}