#pragma once
#include "Entity.h"
#include "ComponentSpecial.h"

namespace FE2D {
    using ComponentTypeID = entt::id_type;

    struct FOR_API ComponentEntry {
        std::function<json(entt::registry&, entt::entity)>              serializeFunc;
        std::function<void(entt::registry&, entt::entity, const json&)> deserializeFunc;

        std::function<const std::string_view&()>                        getNameFunc;

        std::function<bool(entt::registry&, entt::entity)>              startTreeFunc;
        std::function<void(entt::registry&, entt::entity, IMGUI&)>      drawUIFunc;
        std::function<void(entt::registry&, entt::entity)>              removeFunc;
        std::function<void(entt::registry&, entt::entity)>              emplaceFunc;
        std::function<void(Entity&)>                                    drawAddComponentUIFunc;

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
            else {
                entry.serializeFunc = [pretty_name](entt::registry& reg, entt::entity e) {
                    json j;
                    j[pretty_name] = json();
                    return j;
                    };

                entry.deserializeFunc = [pretty_name](entt::registry& reg, entt::entity e, const json& j) {};
            }

            if constexpr (std::is_base_of_v<IDrawableUIComponent, T>) {
                entry.startTreeFunc = [pretty_name](entt::registry& reg, entt::entity e) {
                    constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
                    return ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, pretty_name.data());
                    };

                entry.drawUIFunc = [pretty_name](entt::registry& reg, entt::entity e, IMGUI& imgui) {
                    auto& comp = reg.get<T>(e);
                    comp.DrawUI(imgui);
                    };

                entry.removeFunc = [](entt::registry& reg, entt::entity e) {
                    if (reg.valid(e) && reg.all_of<T>(e))
                        reg.remove<T>(e);
                    };
            }

            if constexpr (std::is_default_constructible_v<T>) {
                entry.emplaceFunc = [](entt::registry& reg, entt::entity e) {
                    if (!reg.all_of<T>(e)) 
                        reg.emplace_or_replace<T>(e);
                    };
            }

            entry.drawAddComponentUIFunc = [pretty_name](Entity& entity) {
                if (entity.HasComponent<T>())
                    return;

                if (ImGui::MenuItem(pretty_name.data())) {
                    entity.AddComponent<T>();
                    ImGui::CloseCurrentPopup();
                }
                };

            entry.getNameFunc = [pretty_name]() { return pretty_name; };

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
