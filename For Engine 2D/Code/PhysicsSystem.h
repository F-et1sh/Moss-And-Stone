#pragma once
#include "ISystemBase.h"

namespace FE2D {
	class FOR_API PhysicsSystem : public ISystemBase {
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() = default;

		void Update();

	private:

	};
}