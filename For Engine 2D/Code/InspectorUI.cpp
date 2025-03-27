#include "forpch.h"
#include "InspectorUI.h"

#include "ActorListUI.h"
#include "ISelectable.h"
#include "IPropertiesDrawable.h"

void FE2D::InspectorUI::Release() {
	m_ResourceUI = nullptr;
	m_ActorListUI = nullptr;

    m_SelectedItem = nullptr;
}

void FE2D::InspectorUI::Initialize(ResourceUI* load_resourceUI, ActorListUI* load_actorListUI) noexcept {
	if (!load_resourceUI)
		FOR_RUNTIME_ERROR("Failed to Initialize InspectorUI\nResourceUI was nullptr");

	if (!load_actorListUI)
		FOR_RUNTIME_ERROR("Failed to Initialize InspectorUI\ActorListUI was nullptr");

	m_ResourceUI = load_resourceUI;
	m_ActorListUI = load_actorListUI;
}

void FE2D::InspectorUI::Update() {
    ImGui::Begin("Inspector");
    
	if (!m_SelectedItem) {
		ImGui::End();
		return;
	}

	if (AActor* actor = dynamic_cast<AActor*>(m_SelectedItem)) {
		
		actor->show_properties();				// actor's custom properties

		this->actor_name	           (actor); // draw renamable actor name
		this->actor_components         (actor);	// draw changable all of actor's components
		this->actor_AddComponent_button(actor);	// draw Add Component button
	}
	else if (Resource* resource = dynamic_cast<Resource*>(m_SelectedItem)) {

		resource->show_properties();			// resource's custom properties

		this->resource_name(resource);	// draw resource's file name
		this->resource_path(resource);	// draw resource's full path
	}

    ImGui::End();
};

void FE2D::InspectorUI::actor_name(AActor* actor) {
	std::string name_buffer = actor->m_Name;

	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	strcpy_s(buffer, sizeof(buffer), name_buffer.c_str());

	if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		name_buffer = std::string(buffer);

	actor->m_Name = name_buffer;
}

void FE2D::InspectorUI::actor_components(AActor* actor) {
	auto& components = actor->getComponents();

	// store deleted components to delete them after for cycle
	// otherwise you will get a runtime error
	FE2D::dynamic_array<size_t> deleted_components;

	for (auto it = components.begin(); it != components.end(); it++) { // running through components
		const std::string component_name = typeid(*(it->second)).name();
		const size_t component_hash_code = typeid(*(it->second)).hash_code();

		const char* component_regular_name =
			ComponentFactory::Instance().getComponentName(component_name).data();

		// if you getting runtime error here
		// most likely you forgot you register a component
		// check the last component you created 
		// and make sure that you have FOR_COMPONENT_REGISTER in the .cpp file of it
		if (ImGui::TreeNodeEx(component_regular_name, ImGuiTreeNodeFlags_DefaultOpen)) {

			if (component_hash_code != typeid(UTransformComponent).hash_code()) {
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
					deleted_components.add(component_hash_code);
			}

			it->second->show_properties();
			ImGui::TreePop();
		}
	}

	for (const auto& hash_code : deleted_components)
		actor->removeComponent(hash_code);
}

void FE2D::InspectorUI::actor_AddComponent_button(AActor* actor) {
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent")) {
		for (const auto& component : ComponentFactory::Instance().getRegisteredComponents()) {
			UComponent* load_component = 
				ComponentFactory::Instance().CreateComponent(component.first);

			if (actor->getComponent(typeid(*load_component).hash_code())) {
				delete load_component;
				continue; // if UComponent is already added to the AActor skip it
			}

			const char* component_regular_name = 
				ComponentFactory::Instance().getComponentName(typeid(*load_component).name()).data();
			
			if (ImGui::MenuItem(component_regular_name))
				actor->addComponent(std::unique_ptr<UComponent>(load_component));
		}

		ImGui::EndPopup();
	}
}

void FE2D::InspectorUI::resource_name(Resource* resource) {
	//const std::filesystem::path resource_path = resource->getFilePath();
	//const std::string name_buffer = "Filename : " + resource_path.filename().string();
	//ImGui::Text(name_buffer.c_str());
}

void FE2D::InspectorUI::resource_path(Resource* resource) {
	//const std::filesystem::path resource_path = resource->getFilePath();
	//const std::string path_buffer = "File path : " + resource_path.string();
	//ImGui::Text(path_buffer.c_str());
}
