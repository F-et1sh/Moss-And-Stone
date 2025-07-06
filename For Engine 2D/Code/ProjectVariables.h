#pragma once
#include "PV_PhysicsLayers.h"

namespace FE2D {
	class FOR_API ProjectVariables {
	public:
		ProjectVariables() = default;
		~ProjectVariables() = default;

		void Save();
		void Load();

		inline PV_PhysicsLayers& getPhysicsLayers()noexcept { return m_PhysicsLayers; }

	private:
		PV_PhysicsLayers m_PhysicsLayers;
	};
}