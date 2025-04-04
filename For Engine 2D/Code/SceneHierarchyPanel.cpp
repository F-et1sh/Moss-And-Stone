#include "forpch.h"
#include "SceneHierarchyPanel.h"

// This needed to disable a security warning on std::strncpy()
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

void FE2D::SceneHierarchyPanel::setContext(Scene* context) {
	m_Context = context;
	m_SelectedEntity = {};
}

void FE2D::SceneHierarchyPanel::OnImGuiRender() {
	ImGui::Begin("Scene Hierarchy");

	if (m_Context) {
		auto view = m_Context->m_Registry.view<entt::entity>();
		for (auto entityID : view) {
			Entity entity{ entityID, m_Context };

			if (entity.HasParent())
				continue;

			DrawEntityNode(entity);
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		// right-click on blank space
        if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

	}
	ImGui::End();

	ImGui::Begin("Properties");
	if (m_SelectedEntity)
		DrawComponents(m_SelectedEntity);

	ImGui::End();
}

void FE2D::SceneHierarchyPanel::DrawEntityNode(Entity entity) {
	auto& tag = entity.GetComponent<TagComponent>().tag;

	ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx(&entity, flags, tag.c_str());
	if (ImGui::IsItemClicked())
		m_SelectedEntity = entity;

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Delete Entity"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	if (opened) {
		for (auto child : entity.GetChildren()) {
			DrawEntityNode(child);
		}

		ImGui::TreePop();
	}

	if (entityDeleted) {
		m_Context->DestroyEntity(entity);
		if (m_SelectedEntity == entity)
			m_SelectedEntity = {};
	}
}

template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
	if (entity.HasComponent<T>()) {
		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight })) {
			ImGui::OpenPopup("Component settings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("Component settings")) {
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open) {
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			entity.RemoveComponent<T>();
	}
}

void FE2D::SceneHierarchyPanel::DrawComponents(Entity entity) {
	if (entity.HasComponent<TagComponent>()) {
		auto& tag = entity.GetComponent<TagComponent>().tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			tag = std::string(buffer);
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent")) {
		DisplayAddComponentEntry<TransformComponent>("Transform");
		DisplayAddComponentEntry<SpriteComponent>("SpriteRenderer");

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {

		FOR_IMGUI.DragVector2("Scale", component.scale);

			//DrawVec3Control("Translation", component.Translation);
			//vec3 rotation = degrees(component.Rotation);
			//DrawVec3Control("Rotation", rotation);
			//component.Rotation = radians(rotation);
			//DrawVec3Control("Scale", component.Scale, 1.0f);
		});

	DrawComponent<SpriteComponent>("SpriteRenderer", entity, [](auto& component) {
		ImGui::DragScalar("Texture", ImGuiDataType_U64, &component.m_TextureIndex);
		
		vec2 size = vec2(component.m_TextureCoords.z, component.m_TextureCoords.w);

		FOR_IMGUI.DragVector2("Size", size);

		component.m_TextureCoords.z = size.x;
		component.m_TextureCoords.w = size.y;
		});
}

template<typename T>
void FE2D::SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
	if (!m_SelectedEntity.HasComponent<T>()) {
		if (ImGui::MenuItem(entryName.c_str())) {
			m_SelectedEntity.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}
}