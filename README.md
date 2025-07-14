# For Engine 2D ( FE2D )
### Moss And Stone

---

**For Engine 2D ( FE2D )** is a custom 2D game engine built with **C++** and **OpenGL 4.6**, designed to create pixel-art games visually using the integrated **FE2D Editor**, while offering full scripting capabilities in C++ via the `Native Scripting`.

## Key Features
- Visual 2D level editor ( FE2D Editor )
- Native C++ scripting support
- Real-time rendering with OpenGL 4.6
- Entity Component System ( ECS )-based architecture
- Pixel art–friendly rendering pipeline

## Used Technologies
- OpenGL 4.6    - rendering
- GLSL          - shaders
- GLFW          - windowing and input
- GLM           - math
- OpenAL        - audio
- ECS           - based on Hazel engine and EnTT
- nlohmann/json - JSON parsing
- STB Image     - image loading
- FreeType      - font loading
- libsndfile    - sound loading
- Boost         - various utilities
- Dear ImGui    - debug GUI and editor interface

---

## FE2D Editor

**FE2D Editor** is a visual scene and game level editor for FE2D.

![FE2D_Editor_Screenshot](/Files/FE2D/FE2D_Editor_Screenshot.png)

---

## Moss And Stone

**Moss And Stone** is the first and flagship game developed using FE2D.  
It's a 2D top-down pixel-art game inspired by titles like **Vampire Survivors** and **Enter the Gungeon**, focusing on stylized visuals and casual gameplay.

---

## Scripting (Native C++)

To create a native script, create two files : `MyScript.h` and `MyScript.cpp`.

### Script registration
**MyScript.cpp**
```cpp
#include "forpch.h"
#include "MyScript.h"

FOR_REGISTER_SCRIPT(MyScript, "My Script")
```

**MyScript.h**
```cpp
#pragma once

class MyScript : public ScriptableEntity {
    GENERATED_BODY(MyScript)

public:
    MyScript()
    {
    }
    ~MyScript()
    {
    }
};
```

---

### Creating fields

- **Component field**
```cpp
ComponentField<TransformComponent> transform;
```

- **Entity field**
```cpp
EntityField player;
```

Don't forget to register your fields

```cpp
void OnRegisterFields() override {
    FOR_REGISTER_FIELD(transform);
    FOR_REGISTER_FIELD(player);
}
```

---

### Accessing fields
```cpp
void OnStart() override {
    transform = this_entity(); // if you assign fields manually registration is not required
    float x = transform->position.x;
}
```

---

### Serialization & Deserialization

```cpp
json Serialize() const override {
    json j;
    FOR_SAVE_FIELD(transform);
    FOR_SAVE_FIELD(player);
    return j;
}

void Deserialize(const json& j) override {
    FOR_LOAD_FIELD(transform);
    FOR_LOAD_FIELD(player);
}
```

---

### Show variable in editor

```cpp
void OnEditorPanel(IMGUI& imgui) override {
    imgui.EntityPayload("Player", player);
}
```

---

### Script lifecycle

- `OnStart()` — called when the scene starts.
- `OnAwake()` — called when the entity is created (e.g. from prefab).
- `OnUpdate(double deltaTime)` — called every frame.
- `OnEnd()` — called when the scene ends.
- `OnDie()` — called when the entity with `HealthComponent` reaches 0 HP.

---

### Utility functions

- `this_entity()` — returns the entity this script is attached to.
- `destroy_this()` — destroys this entity.
- `this_scene()` — returns a reference to the scene.
- `get_resource_manager()` — access to resource manager.
- `get_project_variables()` — access to global project variables.

---

### Prefab creation example

```cpp
ResourceID<Prefab> effect;

void OnAwake() override {
    auto& rm = get_resource_manager();
    auto& prefab = rm.GetResource(effect);
    Entity summoned = prefab.CreateEntity(this_scene());

    auto& summoned_transform = summoned.GetComponent<TransformComponent>();
    summoned_transform.position = GetComponent<TransformComponent>().position;
}

json Serialize() const override {
    json j;
    FOR_SAVE_RESOURCE(effect);
    return j;
}

void Deserialize(const json& j) override {
    FOR_LOAD_RESOURCE(effect);
}

void OnEditorPanel(IMGUI& imgui) override {
    imgui.SelectPrefab(effect);
}
```

---

### Movement logic example

```cpp
void OnUpdate(double deltaTime) override {
    auto& player_transform = player->GetComponent<TransformComponent>();

    vec2 direction = player_transform.position - transform->position;

    if (length(direction) != 0.0f) {
        direction = normalize(direction);
        constexpr float speed = 20.0f;
        transform->position += direction * static_cast<float>(deltaTime) * speed;
    }
}
```

---

### Input handling

**Event Subscription**
```cpp
vec2 mouse_position;

void OnStart() override {
    subscribe_to_event(EventType::MouseMoved, [&](const IEvent& e) {
        const MouseMoved& evt = static_cast<const MouseMoved&>(e);
        mouse_position = evt.position;
        mouse_position.y = this_scene().getRenderContext().getResolution().y - mouse_position.y;
        mouse_position -= this_scene().getRenderContext().getResolution() / vec2(2);
    });
}
```

**Keyboard input**
```cpp
vec2 dir;

void OnUpdate(double deltaTime) override {
    if (is_key_pressed(GLFW_KEY_W)) dir += vec2(0, 1);
    if (is_key_pressed(GLFW_KEY_S)) dir += vec2(0, -1);
    if (is_key_pressed(GLFW_KEY_A)) dir += vec2(-1, 0);
    if (is_key_pressed(GLFW_KEY_D)) dir += vec2(1, 0);
}
```

**Mouse button input**
```cpp
void OnUpdate(double deltaTime) override {
    if (is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
        // shoot
    }
}
```

**Collision**
```cpp
void OnTriggerEnter(Entity other) override {
    // called when two PhysicsComponents overlap and layers are compatible
}
```

---

### Serialization Macros

- `FOR_SAVE_FIELD`, `FOR_SAVE_RESOURCE`, `FOR_SAVE_VALUE`
- `FOR_SAVE_VECTOR_2/3/4`
- `FOR_LOAD_FIELD`, `FOR_LOAD_RESOURCE`, `FOR_LOAD_VALUE`
- `FOR_LOAD_VECTOR_2/3/4`

---

## File Formats

| Extension    | Description                                       |
|--------------|---------------------------------------------------|
| `.fs`        | For Save — metadata for resources				   |
| `.forscene`  | For Scene — scene files						   |
| `.fa`        | For Animation — Aseprite animation				   |
| `.fp`        | For Prefab — prefab files						   |
| `.fpv`       | For Project Variables - contans project variables |

---

## Licenses

This project is **not open-source**.  
For detailed information about usage restrictions, see [`LICENSE.txt`](./LICENSE.txt).

Licenses for third-party libraries used in this project are included in the `build` directory next to the executable ( `.exe` ).

---

© 2025 Farrakh Fattakhov. All rights reserved.