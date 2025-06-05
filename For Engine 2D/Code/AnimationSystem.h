#pragma once
#include "SystemBase.h"

#include "Animation.h"

namespace FE2D {
	class FOR_API AnimationSystem : public SystemBase {
	public:
		AnimationSystem() = default;
		~AnimationSystem() { this->Release(); }

		void Release();
		void Initialize();

		void Render();

	private:

	};
}