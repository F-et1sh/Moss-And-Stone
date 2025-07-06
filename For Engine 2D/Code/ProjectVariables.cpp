#include "forpch.h"
#include "ProjectVariables.h"

void FE2D::ProjectVariables::Serialize() {
	std::filesystem::path path = FOR_PATH.get_engine_data_path() / "project_variables.fs";
	path = FE2D::generate_unique_filename(path);
	
	std::ofstream file(path);
	if (!file.good()) {
		SAY("ERROR : Failed to serialize project variables");
		return;
	}

	
}