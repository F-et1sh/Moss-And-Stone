#pragma once
#include "SystemBase.h"

namespace FE2D {
	class FOR_API PhysicsSystem : public SystemBase {
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() { this->Release(); }

		void Release();
		void Initialize();

		void Update();

	private:

	};
}