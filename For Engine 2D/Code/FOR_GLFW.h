#pragma once

namespace FE2D {
	class FOR_API GLFW {
	public:
		GLFW() = default;
		~GLFW() = default;
	public:
		static void Release()noexcept;
		static inline void Initialize()noexcept;

		static inline void MakeContextCurrent(GLFWwindow* window)noexcept;
		static inline void setViewport(const size_t x, const size_t y, const size_t width, const size_t height)noexcept;
		static inline void setViewport(const vec4& viewport)noexcept;
		enum class DEPTH_FUNC {
			ALWAYS   = GL_ALWAYS,	// The depth test always passes.
			NEVER    = GL_NEVER,	// The depth test never passes.
			LESS     = GL_LESS,		// Passes if the fragment's depth value is less than the stored depth value.
			EQUAL    = GL_EQUAL,	// Passes if the fragment's depth value is equal to the stored depth value.
			LEQUAL   = GL_LEQUAL,	// Passes if the fragment's depth value is less than or equal to the stored depth value.
			GREATER  = GL_GREATER,	// Passes if the fragment's depth value is greater than the stored depth value.
			NOTEQUAL = GL_NOTEQUAL,	// Passes if the fragment's depth value is not equal to the stored depth value.
			GEQUAL   = GL_GEQUAL,	// Passes if the fragment's depth value is greater than or equal to the stored depth value.
		};
		static inline void EnableDepthTest(const DEPTH_FUNC func)noexcept;
		static inline void EnablePolygonOffsetFill(const size_t factor, const size_t units)noexcept;
		enum class BLEND_FUNC {
			ZERO					 = GL_ZERO,						// Factor is equal to 0
			ONE						 = GL_ONE,						// Factor is equal to 1
			SRC_COLOR				 = GL_SRC_COLOR,	            // Factor is equal to the source color vector C-source
			ONE_MINUS_SRC_COLOR		 = GL_ONE_MINUS_SRC_COLOR,		// Factor is equal to 1 minus the source color vector : 1-C-source
			DST_COLOR				 = GL_DST_COLOR,	            // Factor is equal to the destination color vector C-destination
			ONE_MINUS_DST_COLOR		 = GL_ONE_MINUS_DST_COLOR,		// Factor is equal to 1 minus the destination color vector : 1-C-destination
			SRC_ALPHA				 = GL_SRC_ALPHA,	            // Factor is equal to the alpha component of the source color vector C-source
			ONE_MINUS_SRC_ALPHA		 = GL_ONE_MINUS_SRC_ALPHA,	    // Factor is equal to 1-alpha of the source color vector C-source
			DST_ALPHA				 = GL_DST_ALPHA,	            // Factor is equal to the alpha component of the destination color vector C-destination
			ONE_MINUS_DST_ALPHA		 = GL_ONE_MINUS_DST_ALPHA,	    // Factor is equal to 1-alpha of the destination color vector C-destination
			CONSTANT_COLOR			 = GL_CONSTANT_COLOR,	        // Factor is equal to the constant color vector C-constant
			ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,	// Factor is equal to 1 - the constant color vector C-constant
			CONSTANT_ALPHA			 = GL_CONSTANT_ALPHA,	        // Factor is equal to the alpha component of the constant color vector C-constant
			ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,	// Factor is equal to 1-alpha of the constant color vector C-constant
		};
		static inline void EnableBlend(const BLEND_FUNC sfactor, const BLEND_FUNC dfactor)noexcept;
		static inline void SwapInterval(const int interval)noexcept;
		static inline void SwapBuffers(GLFWwindow* window)noexcept;
		static inline const bool WindowShouldClose(GLFWwindow* window)noexcept;
		static inline void DestroyWindow(GLFWwindow* window)noexcept;
		static inline void PollEvents()noexcept;
		static inline void setClipboardString(GLFWwindow* window, const char* string)noexcept;
		static inline const char* getClipboardString(GLFWwindow* window)noexcept;
	};
}