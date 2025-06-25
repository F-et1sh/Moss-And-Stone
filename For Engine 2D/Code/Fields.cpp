#include "forpch.h"
#include "Fields.h"

void FE2D::IField::init(Scene& scene) {
	entity = scene.GetEntityByUUID(uuid);
}
