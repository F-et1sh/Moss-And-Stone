#pragma once
#include "ISystemBase.h"

#include "Animation.h"
#include "AnimationStateNodes.h"

namespace FE2D {
	class FOR_API AnimationSystem : public ISystemBase {
	public:
		AnimationSystem() = default;
		~AnimationSystem() { this->Release(); }

		void Release();
		void Initialize();

		void Render();

	private:
		
	};
}