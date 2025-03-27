#include "forpch.h"
#include "FOR_GLFW.h"

void FE2D::GLFW::Release() noexcept {
    glfwTerminate();
}

inline void FE2D::GLFW::Initialize() noexcept {
    /* Initialize the GFWL Library */
    if (!glfwInit())
        FOR_RUNTIME_ERROR("Failed to Initialize GLFW");

    /* OpenGL Context Vision */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

inline void FE2D::GLFW::MakeContextCurrent(GLFWwindow* window) noexcept {
    glfwMakeContextCurrent(window);
}
inline void FE2D::GLFW::setViewport(const size_t x, const size_t y, const size_t width, const size_t height)noexcept {
    glViewport(x, y, width, height);
}

inline void FE2D::GLFW::EnableDepthTest(const DEPTH_FUNC func) noexcept {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(static_cast<size_t>(func));
}

inline void FE2D::GLFW::EnablePolygonOffsetFill(const size_t factor, const size_t units) noexcept {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(factor, units);
}

inline void FE2D::GLFW::EnableBlend(const BLEND_FUNC sfactor, const BLEND_FUNC dfactor) noexcept {
    glEnable(GL_BLEND);
    glBlendFunc(static_cast<size_t>(sfactor), static_cast<size_t>(dfactor));
}

inline void FE2D::GLFW::SwapInterval(const int interval) noexcept {
    glfwSwapInterval(interval);
}

inline void FE2D::GLFW::SwapBuffers(GLFWwindow* window) noexcept {
    glfwSwapBuffers(window);
}

inline const bool FE2D::GLFW::WindowShouldClose(GLFWwindow* window) noexcept {
    return glfwWindowShouldClose(window);
}

inline void FE2D::GLFW::DestroyWindow(GLFWwindow* window) noexcept{
    glfwDestroyWindow(window);
}

inline void FE2D::GLFW::PollEvents() noexcept{
    glfwPollEvents();
}

inline void FE2D::GLFW::setClipboardString(GLFWwindow* window, const char* string) noexcept{
    glfwSetClipboardString(window, string);
}

inline const char* FE2D::GLFW::getClipboardString(GLFWwindow* window) noexcept {
    return glfwGetClipboardString(window);
}


