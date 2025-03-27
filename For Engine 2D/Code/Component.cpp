#include "forpch.h"
#include "Component.h"

#include "Actor.h"

UComponent* FE2D::UComponent::getComponent(size_t hash_code) {
#ifdef _DEBUG
    if (!m_Actor)
        FOR_RUNTIME_ERROR("AActor was nullptr");
#endif

    if (auto it = m_Actor->getComponent(hash_code))
        return it;

    return nullptr;
}
