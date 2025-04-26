#pragma once
#include "SystemBase.h"

#include "Window.h"

namespace FE2D {
	class FOR_API PlayerControllerSystem : public SystemBase {
	public:
		PlayerControllerSystem() = default;
		~PlayerControllerSystem() { this->Release(); }

		void Release();
		void Initialize();

		void Update();

	private:

	};
}