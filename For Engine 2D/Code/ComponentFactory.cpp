#include "forpch.h"
#include "ComponentFactory.h"

ComponentFactory& FE2D::ComponentFactory::Instance() {
    static ComponentFactory instance;
    return instance;
}
