#include "Application.h"

constexpr glm::vec2 WINDOW_RESOLUTION = glm::vec2(-1, -1);
constexpr std::string_view WINDOW_NAME = "Moss And Stone";
constexpr int WINDOW_MONITOR = 0;

constexpr int SUCCESSFUL_EXIT = 0;
constexpr int FAILED_EXIT = -1;

int main(int argc, char* argv[]) {
	setlocale(0, "Russian");

	std::unique_ptr<FE2D::Application> app = std::make_unique<FE2D::Application>();
	try {
		FOR_PATH.init(argv[0], false); // instance of the PathManager
		app->Initialize(WINDOW_RESOLUTION, WINDOW_NAME, WINDOW_MONITOR);
	}
	catch (const std::exception& e) {

		std::ofstream file("mas_output.txt");
		file << "ERROR : Failed to Initlaize Game Application\n" << e.what();
		file.close();

		return FAILED_EXIT;
	}
	app->Loop();
	app->Release();

	return SUCCESSFUL_EXIT;
}