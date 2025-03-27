#include "forpch.h"
#include "Window.h"

#include "stb_image.h"

void FE2D::Window::Release() {
	if (m_Window) 
		GLFW::DestroyWindow(m_Window);
	m_Window = nullptr;
	m_Monitors = nullptr;
}

void FE2D::Window::Initialize(const vec2& window_size, const std::string& window_name, size_t monitor) {
	/* Find the Monitors */
	int monitors_count = 0;
	m_Monitors = glfwGetMonitors(&monitors_count);
	
	/* Monitor ID Setting */
	if (monitor == -1) m_CurrentMonitor = -1;
	else if (monitor < monitors_count) m_CurrentMonitor = monitor;
	else {
		FOR_RUNTIME_ERROR("Wrong monitor ID : " + std::to_string(monitor) + 
			"\nExists only : " + std::to_string(monitors_count) + 
			"\nFailed to Create GLFW Window");
	}
	
	/* Set the Resolution */
	if (window_size <= vec2(0, 0)) {
		this->FindResolution();
	}
	else m_Resolution = window_size;
	
	/* Create a Window */
	m_Window = glfwCreateWindow(m_Resolution.x, m_Resolution.y, window_name.c_str(), (m_CurrentMonitor == -1) ? NULL : m_Monitors[m_CurrentMonitor], NULL);
	if (!m_Window) {
		FOR_RUNTIME_ERROR("Failed to Create GLFW Window");
	}
	
	/* Make the window's context current */
	GLFW::MakeContextCurrent(m_Window);
	
	/* GLEW Initialize */
	if (glewInit() != GLEW_OK) {
		GLFW::DestroyWindow(m_Window);
		FOR_RUNTIME_ERROR("Failed to Initialize GLEW");
	}
	
	/* Enable Depth Test */
	GLFW::EnableDepthTest(GLFW::DEPTH_FUNC::LESS);
	
	/* Enable Polygon Offset */
	GLFW::EnablePolygonOffsetFill(FOR_POLYGON_FACTOR, FOR_POLYGON_UNITS);
	
	/* Enable Blend Function */
	GLFW::EnableBlend(GLFW::BLEND_FUNC::SRC_ALPHA, GLFW::BLEND_FUNC::ONE_MINUS_SRC_ALPHA);
	
	/* V-syn */
	GLFW::SwapInterval(m_VSyn);

	/* Apply Callbacks at First */
	this->ApplyCallbacks();

	/* Viewport for the Window */
	GLFW::setViewport(0, 0, m_Resolution.x, m_Resolution.y);

	/* Subscribe on Events */
	{
		/* Subscribe on Window Focus Event */
		/* If the Window is not Focused Enable V-Syn | Make Context Current | Apply GLFW Callbacks */
		this->SubscribeOnEvent(Event::EventType::WindowFocus, [this](const Event& e) {
			GLFW::SwapInterval(this->m_VSyn);
			GLFW::MakeContextCurrent(this->m_Window);

			this->ApplyCallbacks();

			if (!FOR_IMGUI.is_initialized())
				return; // if ImGui wasn't Initialized do not initialize it again

			// Reinitialize ImGui after window focus
			try {
				FOR_IMGUI.Initialize(this->m_Window);
			}
			catch (const std::exception& e) {
				SAY("Failed to Reinitilize ImGui after window focus\n" << e.what());
			}
			});

		/* Subscribe on Window Lost Focus Event */
		this->SubscribeOnEvent(Event::EventType::WindowLostFocus, [this](const Event& e) {
			GLFW::SwapInterval(FOR_LOW_OPERATION_MODE);
			});

		/* Subscribe on Window Resized Event */
		this->SubscribeOnEvent(Event::EventType::WindowResized, [this](const Event& e) {
			const WindowResized* resized = static_cast<const WindowResized*>(&e);
			this->m_Resolution = resized->size;

			/* Set the Viewport */
			GLFW::setViewport(0, 0, m_Resolution.x, m_Resolution.y);
			});
	}

	/* Loop until the user closes the window */
	m_IsRunning = true;

	/* Set auto closing on */
	this->setAutoClose(true);

	/*
	std::locale::global(std::locale("en_US.UTF-8"));

    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return -1;
    }

    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cerr << "Failed to create or set OpenAL context" << std::endl;
        if (context) alcDestroyContext(context);
        alcCloseDevice(device);
        return -1;
    }
	*/
}

#pragma region Screen Update

bool FE2D::Window::isOpen() const noexcept { 
	return m_IsRunning;
}

inline void FE2D::Window::ApplyCallbacks()noexcept {
	// Window Events
	{
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				window_instance->m_EventDispatcher.dispatch(WindowClosed());
			}
			});
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				window_instance->m_EventDispatcher.dispatch(WindowResized(vec2(width, height)));
			}
			});
		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				if (focused)
					window_instance->m_EventDispatcher.dispatch(WindowFocus());
				else
					window_instance->m_EventDispatcher.dispatch(WindowLostFocus());
			}
			});
		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				window_instance->m_EventDispatcher.dispatch(WindowMoved(vec2(xpos, ypos)));
			}
			});
	}

	// Keyboard Events
	{
		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				window_instance->m_EventDispatcher.dispatch(TextEntered(codepoint));
			}
			});
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				bool isAltPressed   = mods & GLFW_MOD_ALT;
				bool isCtrlPressed  = mods & GLFW_MOD_CONTROL;
				bool isShiftPressed = mods & GLFW_MOD_SHIFT;
				bool isSuperPressed = mods & GLFW_MOD_SUPER;

				switch (action) {
				case GLFW_PRESS:
					window_instance->m_EventDispatcher.dispatch(KeyPressed(key, scancode, isAltPressed, isCtrlPressed, isShiftPressed, isSuperPressed));
					break;
				case GLFW_RELEASE:
					window_instance->m_EventDispatcher.dispatch(KeyReleased(key, scancode, isAltPressed, isCtrlPressed, isShiftPressed, isSuperPressed));
					break;
				case GLFW_REPEAT:
					window_instance->m_EventDispatcher.dispatch(KeyHeld(key, scancode, isAltPressed, isCtrlPressed, isShiftPressed, isSuperPressed));
					break;
				}
			}
			});
	}

	// Mouse Events
	{
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				window_instance->m_EventDispatcher.dispatch(MouseWheelScrolled(vec2(xoffset, yoffset)));
			}
			});
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				switch (action) {
				case GLFW_PRESS:
					window_instance->m_EventDispatcher.dispatch(MouseButtonPressed(button));
					break;
				case GLFW_RELEASE:
					window_instance->m_EventDispatcher.dispatch(MouseButtonReleased(button));
					break;
				}
			}
			});
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				window_instance->m_EventDispatcher.dispatch(MouseMoved(vec2(xpos, ypos)));
			}
			});
		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				if (entered)
					window_instance->m_EventDispatcher.dispatch(MouseEntered());
				else
					window_instance->m_EventDispatcher.dispatch(MouseLeft());
			}
			});
	}

	// Drop Path
	{
		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int path_count, const char** paths) {
			Window* window_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (window_instance) {
				std::vector<std::wstring> load_paths;
				load_paths.reserve(path_count);
				for (int i = 0; i < path_count; i++) load_paths.push_back(FE2D::string_to_wstring(paths[i]));
				window_instance->m_EventDispatcher.dispatch(DropPath(load_paths));
			}
			});
	}

	glfwSetWindowUserPointer(m_Window, this);
}
inline void FE2D::Window::ClearColor(vec4 color) const noexcept { 
	glClearColor(color.x, color.y, color.z, color.w); 
}
inline void FE2D::Window::ClearColor(float r, float g, float b, float a) const noexcept { 
	glClearColor(r, g, b, a);
}
inline void FE2D::Window::ClearScreen(unsigned int buffer_bit) const noexcept {
	glClear(buffer_bit);
}
inline void FE2D::Window::SwapBuffers() const noexcept { 
	GLFW::SwapBuffers(m_Window);
}
inline void FE2D::Window::PollEvent() {
	GLFW::PollEvents();
}
#pragma endregion

#pragma region get/set

inline const vec2& FE2D::Window::getResolution()const noexcept { return m_Resolution; }
inline const vec2& FE2D::Window::getAspectRatio()const noexcept { return m_AspectRatio; }
inline void        FE2D::Window::FindResolution()noexcept {
	m_Resolution.x = static_cast<float>(glfwGetVideoMode(m_Monitors[m_CurrentMonitor])->width);
	m_Resolution.y = static_cast<float>(glfwGetVideoMode(m_Monitors[m_CurrentMonitor])->height);

	m_AspectRatio = vec2(double(m_Resolution.y) / double(m_Resolution.x), 1.0f);

	m_RefreshRate = glfwGetVideoMode(m_Monitors[m_CurrentMonitor])->refreshRate;
}
inline void        FE2D::Window::FindResolution(const unsigned int load_monitorID)noexcept {
	m_Resolution.x = static_cast<float>(glfwGetVideoMode(m_Monitors[load_monitorID])->width);
	m_Resolution.y = static_cast<float>(glfwGetVideoMode(m_Monitors[load_monitorID])->height);

	m_AspectRatio = vec2(double(m_Resolution.y) / double(m_Resolution.x), 1.0f);

	m_RefreshRate = glfwGetVideoMode(m_Monitors[load_monitorID])->refreshRate;
}
inline void        FE2D::Window::setMaximazed(const bool maximized)noexcept {
	if (maximized)
		glfwSetWindowMonitor(
			m_Window,
			m_Monitors[m_CurrentMonitor],
			0,
			0,
			m_Resolution.x,
			m_Resolution.y,
			m_RefreshRate
		);
	else
		glfwSetWindowMonitor(
			m_Window,
			NULL,
			static_cast<int>(m_Resolution.x / 4),
			static_cast<int>(m_Resolution.y / 4),
			static_cast<int>(m_Resolution.x / 2),
			static_cast<int>(m_Resolution.y / 2),
			m_RefreshRate
		);
}
inline void        FE2D::Window::setIcon(const std::string& path) {
	GLFWimage icon = GLFWimage();
	icon.pixels = stbi_load(path.c_str(), &icon.width, &icon.height, nullptr, 4);
	if (!icon.pixels)
		SAY("ERROR : Failed to Load Default Icon");
	else
		glfwSetWindowIcon(m_Window, 1, &icon);
	stbi_image_free(icon.pixels);
}
inline void        FE2D::Window::setTitle(const std::string& name)const noexcept { glfwSetWindowTitle(m_Window, name.c_str()); }

inline void FE2D::Window::setVSyn(int vsyn) noexcept {
	m_VSyn = vsyn;
	GLFW::SwapInterval(m_VSyn);
}

#pragma endregion

#pragma region FPS Counter

inline const size_t& FE2D::Window::getFPS()noexcept{
	double currentTime = glfwGetTime();
	m_NbFrames++;
	if (currentTime - m_LastTime_FPS >= 1.0f / m_FPS_Speed) {
		m_Current_FPS = m_NbFrames * m_FPS_Speed;
		m_NbFrames = 0;
		m_LastTime_FPS = currentTime;
	}
	return m_Current_FPS;
}

inline void FE2D::Window::Update_DeltaTime() noexcept {
	auto currentTime = _clock::now();
	m_DeltaTime = (currentTime - m_LastTime_DeltaTime);
	m_LastTime_DeltaTime = currentTime;
}

#pragma endregion

void FE2D::Window::setAutoClose(bool auto_close) {
	if (auto_close && !m_WindowClose_EventIndex) {

		m_WindowClose_EventIndex = this->SubscribeOnEvent(Event::EventType::WindowClosed, [&](const Event& e) {
			this->m_IsRunning = false;
			});
	}
	else {
		this->UnsubscribeOnEvent(Event::EventType::WindowClosed, m_WindowClose_EventIndex);
	}
}