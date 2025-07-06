#include "forpch.h"
#include "ProjectVariables.h"

void FE2D::ProjectVariables::Save() {
	std::filesystem::path path = FOR_PATH.get_engine_data_path() / "project_variables.fpv";
	
	std::ofstream file(path);
	if (!file.good()) {
		SAY("ERROR : Failed to serialize project variables");
		return;
	}

	json j;
	j["PhysicsLayers"] = m_PhysicsLayers.Serialize();

#ifdef _DEBUG
	file << j.dump(4);
#else
	file << j;
#endif
}

void FE2D::ProjectVariables::Load() {
	std::filesystem::path path = FOR_PATH.get_engine_data_path() / "project_variables.fpv";

	std::ifstream file(path);
	if (!file.good()) {
		SAY("ERROR : Failed to deserialize project variables");
		return;
	}

	json j;
	file >> j;

	if (!j.contains("PhysicsLayers")) {
		SAY("ERROR : Failed to deserialize project variables : PhysicsLayers");
	}
	else m_PhysicsLayers.Deserialize(j["PhysicsLayers"]);
}