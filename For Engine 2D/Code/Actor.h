#pragma once
#include "ISelectable.h"
#include "UTransformComponent.h"

namespace FE2D {
    class FOR_API AActor : public ISelectable {
    public:
        using _ComponentRef = std::unique_ptr<UComponent>;
        
        // size_t - typeid(T).hash_code()
        // UComponent* - smart pointer of UComponent
        using _Components = std::vector<std::pair<size_t, _ComponentRef>>;
    public:
        AActor() { this->addComponent<UTransformComponent>(); }
        AActor(const std::string& name) : m_Name(name) { this->addComponent<UTransformComponent>(); }

        ~AActor() = default;
    public:
        AActor(AActor&& other) noexcept
            : m_Name(std::move(other.m_Name)),
            m_IsVisible(other.m_IsVisible),
            m_Components(std::move(other.m_Components)),
            m_Childrens(std::move(other.m_Childrens)) {
        }
    public:
        virtual bool SerializeToFolder(const std::filesystem::path& actor_folder_path)const;
        virtual bool DeserializeFromFolder(const std::filesystem::path& actor_folder_path);

    public:
        void show_properties()override {
            // ..
        }

        void OnSelected()override {
            ISelectable::OnSelected();
            // ..
        }
        void OnDeselected()override {
            ISelectable::OnDeselected();
            // ..
        }

    public:
        std::string m_Name = "New Actor";

        bool m_IsVisible = true;  // If it's false the actor will skip Draw() function
    public:
        // Call Release() funtion of Components and clear them
        virtual void Release();

        // Initialize all UComponents after adding
        virtual void Initialize();

        // Check if the UComponents are enabled and update all of them
        virtual void Update(double _deltaTime, Input& input);
    public:
        // Add a UComponent without calling override function of UComponent - Initialize()
        // You can't add a UComponent that is already added
        template<typename T>
        T* addComponent() {
            // Check for inheritance from Component
            static_assert(std::is_base_of<UComponent, T>::value, "ERROR : T must be derived from UComponent");

            static const size_t hash_code = typeid(T).hash_code();
            static const std::string type_name = typeid(T).name();

            if (this->find_component(hash_code) == m_Components.end()) { // check if the component not added before
                UComponent* component = new T();
                component->setActor(this);
                m_Components.push_back({ hash_code, std::unique_ptr<UComponent>(component) });
                return static_cast<T*>(component);
            }
            else {
                FOR_RUNTIME_ERROR("Failed to Add a Component to an Actor\nComponent is already added\nComponent Name : " + type_name);
            }
        }

        // Add an existing UComponent without calling override function of UComponent - Initialize()
        // You can't add a UComponent that is already added
        void addComponent(_ComponentRef component) {
            static const size_t hash_code = typeid(*component).hash_code();
            if (this->find_component(hash_code) == m_Components.end()) { // check if the Component not Added before

                component->setActor(this);
                m_Components.emplace_back(hash_code, std::move(component));
            }
            else {
                FOR_RUNTIME_ERROR("Failed to Add a Component to an Actor\nComponent is already added\nComponent Name : " + std::string(typeid(*component).name()));
            }
        }

        // Get a UComponent. You can change it however you want
        // This function CAN return nullptr
        template<typename T>
        T* getComponent() {
            static_assert(std::is_base_of<UComponent, T>::value, "ERROR : T must be derived from UComponent");
            static const size_t hash_code = typeid(T).hash_code();
            return static_cast<T*>(this->getComponent(hash_code));
        }

        // Get a UComponent pointer. You can change it however you want
        // This function CAN return nullptr
        UComponent* getComponent(size_t hash_code) {
            auto it = this->find_component(hash_code);
            if (it != m_Components.end()) {
                return it->second.get();
            }
            return nullptr;
        }

        // Get std::map of Components. You can change it however you want
        // size_t - typeid(T).hash_code() | UComponent* - pointer of Component
        _Components& getComponents()noexcept {
            return m_Components;
        }

        // Remove a Component with Release Calling
        template<typename T>
        void removeComponent() {
            static_assert(std::is_base_of<UComponent, T>::value, "ERROR : T must be derived from UComponent");
            static_assert(!std::is_same<T, UTransformComponent>::value, "ERROR : Can't remove UTransformComponent");

            static const size_t hash_code = typeid(T).hash_code();

            auto it = this->find_component<T>();

            if (it != m_Components.end()) {
                it->second->Release();
                m_Components.erase(it);
            }
        }

        // Remove a Component with Release Calling
        void removeComponent(size_t hash_code) {
            if (hash_code == typeid(UTransformComponent).hash_code())
                FOR_RUNTIME_ERROR("ERROR : Cannot remove UTransformComponent");

            auto it = this->find_component(hash_code);

            if (it != m_Components.end()) { // if the component added to the array
                it->second->Release();
                m_Components.erase(it);
            }
        }
    private:
        template<typename T>
        auto find_component() -> _Components::iterator {
            static const size_t hash_code = typeid(T).hash_code();
            return std::find_if(m_Components.begin(), m_Components.end(),
                [hash_code](const std::pair<size_t, _ComponentRef>& pair) {
                    return pair.first == hash_code;
                });
        }

        auto find_component(size_t hash_code) -> _Components::iterator {
            return std::find_if(m_Components.begin(), m_Components.end(),
                [hash_code](const std::pair<size_t, _ComponentRef>& pair) {
                    return pair.first == hash_code;
                });
        }
    public:
        inline void addChildren(AActor* load_children) { m_Childrens.push_back(load_children); }
        inline std::vector<AActor*>& getChildrens() noexcept { return m_Childrens; }
    protected:
        _Components m_Components;
    protected:
        std::vector<AActor*> m_Childrens;
    };
}