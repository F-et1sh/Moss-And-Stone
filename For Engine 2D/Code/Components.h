#pragma once
#include "UUID.h"
#include "Entity.h"
#include "ScriptableEntity.h"
#include "AnimationStateNodes.h"

namespace FE2D {
    /* start of components */
    
    struct FOR_API IDComponent {
        FE2D::UUID id;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(FE2D::UUID id) : id(id) {}
    };

    struct FOR_API TagComponent {
        std::string tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : tag(tag) {}
    };

    struct FOR_API TransformComponent {
        vec2 position = vec2();

        int layer = 0;

        // flag for auto sprite sorting by Y-axis
        bool auto_sort = false;

        vec2 scale = vec2(1.0f, 1.0f);

        float rotation = 0.0f;
        vec2  origin = vec2();

        inline operator mat4()const noexcept {
            mat4 matrix = mat4(1.0f);

            // translation
            {
                matrix = glm::translate(matrix, vec3(position,                // position
                    (auto_sort ? (-position.y * std::abs(scale.y) / 2) : 0) + // auto-sorting
                    layer));                                                  // layer
            }

            // rotation
            {
                // set origin
                matrix = glm::translate(matrix, vec3(-origin * scale, 0));

                // apply rotation
                matrix = glm::rotate(matrix, glm::radians(rotation), vec3(0.0f, 0.0f, -1.0f));

                // reset origin
                matrix = glm::translate(matrix, vec3(origin * scale, 0));
            }

            // scaling
            {
                matrix = glm::scale(matrix, vec3(scale, 1.0f));
            }

            return matrix;
        }

        TransformComponent() = default;
        ~TransformComponent() = default;

        TransformComponent(const TransformComponent&) = default;
    };

    struct FOR_API SpriteComponent {
        ResourceID<Texture> texture;

        vec4 frame = vec4();

        bool flip_x = false;
        bool flip_y = false;

        SpriteComponent() = default;
        ~SpriteComponent() = default;
        SpriteComponent(const SpriteComponent&) = default;
    };

    struct FOR_API RelationshipComponent {
        FE2D::UUID parent = FE2D::UUID(0);
        std::vector<FE2D::UUID> children;

        RelationshipComponent() = default;
        ~RelationshipComponent() = default;

        RelationshipComponent(const RelationshipComponent&) = default;
    };

    struct FOR_API CameraComponent {
        vec4 clear_color = vec4();

        CameraComponent() = default;
        ~CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct FOR_API PhysicsComponent {
        vec2 position = vec2();
        vec2 size = vec2(50, 50);

        // 0 - no bounce
        // 1 - full bounce
        float bounce_factor = 0.0f;

        float mass = 1.0f;

        bool is_trigger = false;
        bool is_static = false;

        vec2 velocity = vec2();

        uint8_t layer = 0;

        std::vector<Entity> entities_in;

        PhysicsComponent() = default;
        ~PhysicsComponent() = default;
        PhysicsComponent(const PhysicsComponent&) = default;
    };

    struct FOR_API AnimatorComponent {
        size_t current_state = 0;
        float current_time = 0.0f;

        std::vector<std::unique_ptr<IStateNode>> states;
        std::vector<AnimationTransition> transitions;
        // name | parameter
        std::unordered_map<std::string, AnimationParameter> parameters;

        inline void setBoolean(const std::string& name, bool value) { this->set_value(name, value); }
        inline void setFloat(const std::string& name, float value) { this->set_value(name, value); }
        inline void setInteger(const std::string& name, int value) { this->set_value(name, value); }
        inline void callTrigger(const std::string& name, float time = 0.5f) {
            auto it = parameters.find(name);
            if (it == parameters.end()) return;

            if (std::holds_alternative<Trigger>(it->second.value))
                std::get<Trigger>(it->second.value).trigger(time);
        }

        inline bool isTriggered(const std::string& name) {
            auto it = parameters.find(name);
            if (it == parameters.end()) return false;

            if (std::holds_alternative<Trigger>(it->second.value))
                return std::get<Trigger>(it->second.value).is_triggered();
        }
        inline bool isAnimationPlaying(const std::string& name) {
            auto it = std::find_if(states.begin(), states.end(), [name](const std::unique_ptr<IStateNode>& e) { return e->name == name; });
            if (it != states.end())
                if (std::distance(states.begin(), it) == current_state) return true;
            return false;
        }

        AnimatorComponent() = default;
        ~AnimatorComponent() = default;

        AnimatorComponent(const AnimatorComponent& other) {
            current_state = other.current_state;
            current_time = other.current_time;
            transitions = other.transitions;
            parameters = other.parameters;

            states.reserve(other.states.size());
            for (const auto& state : other.states) {
				if (state) states.emplace_back(state->clone());
            }
        }
        AnimatorComponent& operator=(const AnimatorComponent& other) {
            if (this == &other) return *this;

            states.clear();

            current_state = other.current_state;
            current_time = other.current_time;
            transitions = other.transitions;
            parameters = other.parameters;

            states.reserve(other.states.size());
            for (const auto& state : other.states) {
                if (state) states.emplace_back(state->clone());
            }

            return *this;
        }

        AnimatorComponent(AnimatorComponent&&) noexcept = default;
        AnimatorComponent& operator=(AnimatorComponent&&) noexcept = default;

    private:
        template<typename T> requires (std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, int> || std::is_same_v<T, Trigger>)
        inline void set_value(const std::string& n, T v) {
            auto it = parameters.find(n);
            if (it == parameters.end()) return;

            if (std::holds_alternative<T>(it->second.value))
                it->second.value = v;
        }
    };

    struct FOR_API NativeScriptComponent {
        std::unique_ptr<ScriptableEntity> instance = nullptr;
        std::string script_name;

        NativeScriptComponent() = default;
        ~NativeScriptComponent() = default;

        NativeScriptComponent(const NativeScriptComponent& other) {
            if (other.instance) {
                instance = other.instance->clone();
                script_name = other.script_name;
            }
        }

        NativeScriptComponent& operator=(const NativeScriptComponent& other) {
            if (this != &other) {
                if (other.instance) {
                    instance = other.instance->clone();
                    script_name = other.script_name;
                }
                else {
                    instance.reset();
                    script_name.clear();
                }
            }
            return *this;
        }

        NativeScriptComponent(NativeScriptComponent&&) noexcept = default;
        NativeScriptComponent& operator=(NativeScriptComponent&&) noexcept = default;
    };

    struct FOR_API HealthComponent {
        int max_health = 0;
        int current_health = 0;
        bool is_dead = false;

        inline void set_new_health(int health)noexcept { max_health = health; current_health = health; }

        inline void heal(int heal_by)noexcept { current_health += heal_by; }
        inline void take_damage(int damage)noexcept { current_health -= damage; }

        HealthComponent(int health) : max_health(health), current_health(health) {}

        HealthComponent() = default;
        ~HealthComponent() = default;
        HealthComponent(const HealthComponent&) = default;
    };

    struct FOR_API TilemapComponent {
        size_t width  = 10;
        size_t height = 10;

        bool is_updated = false;
        
        // this uint8_t needed to find tile info in the array ( TilemapComponnet::tile_info )
        // there is a limit of tiles
        // maximum can be 64 types of tiles, because of uint64_t
        std::vector<uint8_t> tiles;

        struct DisplayTile {
            uint8_t tile_id;
            ivec4 frame;
        };

        std::vector<DisplayTile> display_tiles;

        struct TileInfo {
            ivec4 frame;
            uint8_t type = 0;
        };

        struct TileType {
            uint64_t mask;
            ResourceID<Texture> texture_atlas;
        };

        std::vector<TileInfo> tile_info;
        std::vector<TileType> tile_types;

        void set_compatible(uint8_t a, uint8_t b, bool value) {
            if (a >= tile_types.size() || b >= tile_types.size()) return;

            if (value) {
                tile_types[a].mask |= (1ull << b);
                tile_types[b].mask |= (1ull << a);
            }
            else {
                tile_types[a].mask &= ~(1ull << b);
                tile_types[b].mask &= ~(1ull << a);
            }
        }

        bool is_compatible(uint8_t a, uint8_t b) const {
            if (a >= tile_types.size() || b >= tile_types.size()) return false;
            return (tile_types[a].mask & (1ull << b)) != 0;
        }

        static glm::ivec4 get_frame(bool lb, bool rb, bool rt, bool lt) {
            uint8_t mask =
                (lb ? 1 : 0) << 0 |  // 0001
                (rb ? 1 : 0) << 1 |  // 0010
                (rt ? 1 : 0) << 2 |  // 0100
                (lt ? 1 : 0) << 3;   // 1000

            static const uint8_t lut[16] = {
                15, // 0000
                14, // 0001 lb
                13, // 0010 rb
                12, // 0011 lb+rb
                11, // 0100 rt
                10, // 0101 lb+rt
                 9, // 0110 rb+rt
                 8, // 0111 lb+rb+rt
                 7, // 1000 lt
                 6, // 1001 lb+lt
                 5, // 1010 rb+lt
                 4, // 1011 lb+rb+lt
                 3, // 1100 rt+lt
                 2, // 1101 lb+rt+lt
                 1, // 1110 rb+rt+lt
                 0  // 1111 all
            };

            uint8_t index = lut[mask];

            int x = (index % 4) * 16;
            int y = (index / 4) * 16;

            return glm::ivec4(x, y, 16, 16);
        }

        TilemapComponent() = default;
        ~TilemapComponent() = default;
        TilemapComponent(const TilemapComponent&) = default;
    };

    /* end of components */

    //

    /* start of effects */

    // Pattern of effect naming : E_NameComponent
    // E - Effect
    // Effects are components, which handling by EffectsSystem

    struct FOR_API E_DamageComponent {
        int damage = 0;

        E_DamageComponent(int damage) : damage(damage) {}
            
        E_DamageComponent() = default;
        ~E_DamageComponent() = default;
        E_DamageComponent(const E_DamageComponent&) = default;
    };

    struct FOR_API E_FreezeComponent {
        float duration = 0;

        E_FreezeComponent(float duration) : duration(duration) {}

        E_FreezeComponent() = default;
        ~E_FreezeComponent() = default;
        E_FreezeComponent(const E_FreezeComponent&) = default;
    };

    /* end of effects */

    //

    /* start of component traits */

    template<typename... Components>
    struct FOR_API ComponentGroup 
    {
    };

    using AllComponentsGroup = ComponentGroup<
        IDComponent,
        TagComponent,
        TransformComponent,
        SpriteComponent,
        RelationshipComponent,
        CameraComponent,
        PhysicsComponent,
        AnimatorComponent,
        NativeScriptComponent,
        HealthComponent,
        TilemapComponent,
        E_DamageComponent,
        E_FreezeComponent>;

    template<typename Group>
    struct FOR_API ComponentsHelper;

    template<typename... Components>
    struct FOR_API ComponentsHelper<ComponentGroup<Components...>> {
        using variant = std::variant<Components...>;

        inline static void collect_from_entity(Entity entity, std::vector<variant>& out) {
            CollectFromEntity<Components...>(entity, out);
        }

    private:
        template<typename T, typename... Rest>
        inline static void CollectFromEntity(Entity entity, std::vector<variant>& out) {
            if (entity.HasComponent<T>())
                out.emplace_back(entity.GetComponent<T>());
            if constexpr (sizeof...(Rest) > 0)
                CollectFromEntity<Rest...>(entity, out);
        }
    };

    using FOR_COMPONENTS_HELPER = ComponentsHelper<AllComponentsGroup>;
    using ComponentsVariant = FOR_COMPONENTS_HELPER::variant;

    /* end of component traits */
}
