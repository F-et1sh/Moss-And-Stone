#pragma once

namespace FE2D {
	class FOR_API HealthSystem : public ISystemBase {
	public:
		HealthSystem() = default;
		~HealthSystem() = default;

		void Update();
	};
}