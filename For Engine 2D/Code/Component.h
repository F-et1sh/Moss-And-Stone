#pragma once
#include "Serializable.h"
#include "IPropertiesDrawable.h"
#include "Input.h"

namespace FE2D {
    class FOR_API AActor;

    class FOR_API SceneManager;

    class FOR_API UComponent : public Serializable, public IPropertiesDrawable {
    public:
        UComponent() = default;
        virtual ~UComponent() = default;

        virtual void Release() {}
        virtual bool Initialize() { return true; }

        virtual void OnInput(Input& input) {}

        virtual void Update(double _deltaTime) {}

        // This virtual function is called every frame after Render() function
        // If you want to change the scene using your component
        // create a flag, override this function and 
        // call SceneManager::LoadScene() if the flag is true
        virtual void LoadScene(SceneManager& sceneManager) {}

    private:
        AActor* m_Actor = nullptr;
    public:
        void setActor(AActor* load_actor_ref) {
            if (!load_actor_ref)
                FOR_RUNTIME_ERROR("Failed to set an AActor in UComponent\nAActor was nullptr");
            m_Actor = load_actor_ref;
        }

        AActor* getActor()const noexcept {
            return m_Actor;
        }

        // Get a UComponent. You can change it however you want
        template<typename T>
        T* getComponent() {
            static_assert(std::is_base_of<UComponent, T>::value, "ERROR : T must be derived from UComponent");

            const size_t hash_code = typeid(T).hash_code();

            return static_cast<T*>(this->getComponent(hash_code));
        }

        // Get a UComponent pointer. You can change it however you want
        UComponent* getComponent(size_t hash_code);
        
    public:
        // Is this component enabled
        // you can change this value in InspectorUI
        bool m_IsEnabled = true;
    };
#define FOR_COMPONENT_REGISTER(type, name) static FE2D::ComponentRegistrar<type> type##_register(name);
}