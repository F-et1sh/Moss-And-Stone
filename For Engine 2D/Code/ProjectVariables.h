#pragma once
#include "PV_PhysicsLayer.h"

namespace FE2D {
	class FOR_API ProjectVariables {
	public:
		ProjectVariables() = default;
		~ProjectVariables() = default;

		void Serialize();
		void Deserialize();

		inline PV_PhysicsLayer& getPhysicsLayerManager()noexcept { return m_PhysicsLayerManager; }

	private:
		PV_PhysicsLayer m_PhysicsLayerManager;
	};
}