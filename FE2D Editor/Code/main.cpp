#include "EditorApplication.h"

constexpr vec2 WINDOW_RESOLUTION = vec2(1920, 1080);
constexpr std::string_view WINDOW_NAME = "FE2D Editor";
constexpr int WINDOW_MONITOR = -1; // not fullscreen

constexpr int SUCCESSFUL_EXIT = 0;
constexpr int FAILED_EXIT = -1;

#include "Test.h"

int main(int argc, char* argv[]) {
	//setlocale(0, "Russian");

	std::unique_ptr<FE2D::EditorApplication> editor_app = std::make_unique<FE2D::EditorApplication>();
	try {
		FOR_PATH.init(argv[0], true); // instance of the PathManager
		editor_app->Initialize(WINDOW_RESOLUTION, WINDOW_NAME, WINDOW_MONITOR);
	}
	catch (const std::exception& e) {

		SAY(e.what());

		std::ofstream file("output.txt");
		file << "ERROR : Failed to Initlaize Editor Application\n" << e.what();
		file.close();

		return FAILED_EXIT;
	}
	editor_app->Loop();
	editor_app->Release();

	return SUCCESSFUL_EXIT;
}

// FPS : if you feel, that FPS is low, check for systems. They use SystemBase as a base class to inherit. You can remove vtable and FPS will grow up