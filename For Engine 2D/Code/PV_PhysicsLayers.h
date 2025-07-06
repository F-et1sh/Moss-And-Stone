#pragma once
#include "IProjectVariable.h"

namespace FE2D {
	class FOR_API PV_PhysicsLayers : public IProjectVariable {
	public:
		PV_PhysicsLayers() = default;
		~PV_PhysicsLayers() = default;

		inline static constexpr uint32_t FOR_PHYSICS_LAYERS_COUNT = 32;

		json Serialize()const override;
		void Deserialize(const json& j)override;

        inline std::optional<uint8_t> find(const std::string& name) const {
            for (uint8_t i = 0; i < FOR_PHYSICS_LAYERS_COUNT; i++)
                if (m_Layers[i].name == name) return i;
            return std::nullopt;
        }

        inline void rename(uint8_t id, const std::string& new_name) {
            m_Layers[id].name = new_name;
        }

        inline void setCollision(uint8_t a, uint8_t b, bool on) {
            if (on) {
                m_Layers[a].mask |= (1u << b);
                m_Layers[b].mask |= (1u << a);
            }
            else {
                m_Layers[a].mask &= ~(1u << b);
                m_Layers[b].mask &= ~(1u << a);
            }
        }

        inline bool canCollide(uint8_t a, uint8_t b)const { return (m_Layers[a].mask >> b) & 1u; }

        bool createLayer(const std::string& name);
        bool removeLayer(int id);

		inline auto& getLayers()noexcept { return m_Layers; }

	private:

		struct LayerInfo {
			std::string name;
			uint32_t mask;
		};

		std::array<LayerInfo, FOR_PHYSICS_LAYERS_COUNT> m_Layers;
        uint32_t m_NextAvailableBit = 0;
	};
}