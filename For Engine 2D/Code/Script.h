#pragma once

namespace FE2D {
	class FOR_API Script : public ScriptableEntity {
	public:
		Script() { SAY("Created"); }
		~Script() { SAY("Deleted"); }

		ComponentField<TransformComponent> transform;
		Entity entity;

		void OnStart()override {

		}

		void OnUpdate(double deltaTime)override {
			
		}

		json Serialize()const override{
			json j;
			j["transform"] = transform.entity.GetUUID().ToString();
			j["entity"] = entity.GetUUID().ToString();
			return j;
		}

		void Deserialize(const json& j)override {
			if (!j.contains("transform"))
				return;

			transform.entity = this->m_Scene->GetEntityByUUID(FE2D::UUID(j["transform"].get<std::string>()));
			entity = this->m_Scene->GetEntityByUUID(FE2D::UUID(j["entity"].get<std::string>()));
		}

		void OnEditorPanel(IMGUI& imgui)override {
			imgui.ComponentPayload(transform);
			imgui.EntityPayload(entity);
		}
	};
}