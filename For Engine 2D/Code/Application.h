#pragma once
#include <forpch.h>

#include "Window.h"
#include "SceneManager.h"
#include "Input.h"

namespace FE2D {
	class FOR_API Application {
	public:
		Application() = default;
		~Application() = default;
	public:
		virtual void Release();
		virtual void Initialize(
			const vec2& window_resolution = vec2(800, 600),
			const std::string_view& window_name = "For Default Application",
			const size_t& monitor = -1);
		virtual void Loop();

	public:
		inline void StartGameSession() { this->m_SceneManager.StartGameSession(); }
		inline void StopGameSession() { this->m_SceneManager.StopGameSession(); }

	public:
		inline Window		  * getWindow		  ()noexcept { return &m_Window					    ; }
		inline ResourceManager* getResourceManager()noexcept { return &m_ResourceManager			; }
		inline ActorList	  * getActorList	  ()noexcept { return  m_SceneManager.getActorList(); }
		inline Input	      * getInput		  ()noexcept { return &m_Input; }

	protected:
		Window m_Window;
		Input m_Input;

	protected:
		ResourceManager m_ResourceManager;
		SceneManager m_SceneManager;
	};
}