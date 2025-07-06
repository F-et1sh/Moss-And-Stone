#pragma once
#include "IProjectVariable.h"

namespace FE2D {
	class FOR_API PV_PhysicsLayer : public IProjectVariable {
	public:
		PV_PhysicsLayer() = default;
		~PV_PhysicsLayer() = default;

		json Serialize()const override;
		void Deserialize(const json& j)override;

		inline auto& getLayerMap()noexcept { return m_LayerMap; }

	private:
		inline static constexpr uint32_t MaxLayers = 32;

		struct LayerInfo {
			std::string name;
			uint32_t mask;
		};

		std::array<LayerInfo, MaxLayers> m_LayerMap;
	};
}