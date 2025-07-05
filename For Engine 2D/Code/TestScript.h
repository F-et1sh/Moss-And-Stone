#pragma once

class TestScript : public ScriptableEntity {
	
	GENERATED_BODY(TestScript)

	void OnRegisterFields()override{}

public:
	TestScript() { SAY("TestScript : Created"); }
	~TestScript() { SAY("TestScript : Deleted"); }


	void OnStart()override {
		SAY("TestScript : Started");
	}

	void OnUpdate(double deltaTime)override {
		SAY("TestScript : Updated");
	}

	json Serialize()const override {
		SAY("TestScript : Serialized");
		return json();
	}

	void Deserialize(const json& j)override {
		SAY("TestScript : Deserialized");
	}

	void OnEditorPanel(IMGUI& imgui)override {
		ImGui::Text("TestScript : Editing");
	}
};
