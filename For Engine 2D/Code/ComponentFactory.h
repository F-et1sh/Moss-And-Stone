#pragma once

namespace FE2D {
    struct FOR_API ISerializableComponent {
        virtual json Serialize()const { return {}; }
        virtual void Deserialize(const json& j) {}

        virtual ~ISerializableComponent() = default;
    };

    struct FOR_API IDrawableUIComponent {
        virtual void DrawUI() = 0;

        virtual ~IDrawableUIComponent() = default;
    };
}

namespace FE2D {
    using ComponentTypeID = entt::id_type;

    struct FOR_API ComponentEntry {
        std::function<json(entt::registry&, entt::entity)> serializeFunc;
        std::function<void(entt::registry&, entt::entity, const json&)> deserializeFunc;
        std::function<void(entt::registry&, entt::entity)> drawUIFunc;
        std::function<void(entt::registry&, entt::entity)> emplaceFunc;

        ComponentEntry() = default;
        ~ComponentEntry() = default;
    };

    class FOR_API ComponentFactory {
    public:
        ComponentFactory() = default;
        ~ComponentFactory() = default;
    public:
        static ComponentFactory& Instance();

        template<typename T>
        inline void RegisterComponent(const std::string_view& pretty_name) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            ComponentEntry entry;

            if constexpr (std::is_base_of_v<ISerializableComponent, T>) {
                entry.serializeFunc = [pretty_name](entt::registry& reg, entt::entity e) {
                    json j;
                    auto& comp = reg.get<T>(e);
                    j[pretty_name] = comp.Serialize();
                    return j;
                    };

                entry.deserializeFunc = [pretty_name](entt::registry& reg, entt::entity e, const json& j) {
                    if (!j.contains(pretty_name))
                        return;

                    auto& comp = reg.get<T>(e);
                    comp.Deserialize(j.at(pretty_name));
                    };
            }

            if constexpr (std::is_base_of_v<IDrawableUIComponent, T>) {
                entry.drawUIFunc = [](entt::registry& reg, entt::entity e) {
                    auto& comp = reg.get<T>(e);
                    comp.DrawUI();
                    };
            }

            if constexpr (std::is_default_constructible_v<T>) {
                entry.emplaceFunc = [](entt::registry& reg, entt::entity e) {
                    if (!reg.all_of<T>(e)) 
                        reg.emplace<T>(e);
                    };
            }

            auto hash = entt::type_id<T>().hash();
            m_RegistredComponents[hash] = std::move(entry);
        }

        const std::unordered_map<ComponentTypeID, ComponentEntry>& getRegistredComponents()const noexcept{
            return m_RegistredComponents;
        }

    private:
        std::unordered_map<ComponentTypeID, ComponentEntry> m_RegistredComponents;
    private:
        mutable std::mutex m_Mutex;
    };
}
