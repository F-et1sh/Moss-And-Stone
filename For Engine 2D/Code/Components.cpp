#include "forpch.h"
#include "Components.h"

#include "ComponentRegistrar.h"

FOR_COMPONENT_REGISTER(IDComponent, "UUID");
FOR_COMPONENT_REGISTER(TagComponent, "Tag")
FOR_COMPONENT_REGISTER(TransformComponent, "Transform")
FOR_COMPONENT_REGISTER(SpriteComponent, "SpriteRenderer")
FOR_COMPONENT_REGISTER(RelationshipComponent, "Relationship")
FOR_COMPONENT_REGISTER(CameraComponent, "Camera")