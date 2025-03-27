#pragma once
#include "Window.h"

namespace FE2D {
	class FOR_API Input {
	public:
		Input() = default;
		~Input() = default;

		void Release();
		void Initialize(Window* window);
		
		int k = 0;

	private:
		Window* m_Window = nullptr;
	};
}