#pragma once
#include "ResourceManager.h"

namespace FE2D {
	class FOR_API IMGUI {
	public:
		IMGUI() = default;
		~IMGUI() = default;
	public:
		static IMGUI& Instance() {
			static IMGUI instance;
			return instance;
		}
	public:
		static constexpr const char* GLSL_VERSION = "#version 460";
		static constexpr float DEFAULT_PIXEL_SIZE = 20.0f;
	private:
		ResourceManager* m_ResourceManager = nullptr;
	private:
		bool m_IsInitialized = false;
	public:
		bool is_initialized()const noexcept { return m_IsInitialized; }
	public:
		void Release();
		void Initialize(GLFWwindow* window_reference);
		void setResourceManager(ResourceManager* resource_manager)noexcept;
		ResourceManager* getResourceManager()const noexcept {
			return m_ResourceManager;
		}

		inline void BeginFrame();
		inline void EndFrame();

		void StartDockSpace();

		void CheckBox   (const std::string& label, bool & value , float columnWidth = 100.0f);
		void DragInt    (const std::string& label, int  & value , float columnWidth = 100.0f);
		void DragFloat  (const std::string& label, float& value , float columnWidth = 100.0f);
		void DragVector2(const std::string& label, vec2 & values, float resetValue = 0.0f, float columnWidth = 100.0f);
		
		void SelectTexture(const std::string& label, size_t& index);
	};
	inline static IMGUI& FOR_IMGUI = FOR_IMGUI.Instance();
}