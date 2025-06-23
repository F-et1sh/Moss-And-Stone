#pragma once

// C/C++
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <optional>
#include <variant>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <map>
#include <typeindex>
#include <future>
#include <memory>
#include <fstream>
#include <sstream>
#include <mutex>
#include <shared_mutex>
#include <cstdint>
#include <stdexcept>
#include <direct.h>
#include <limits.h>
#include <cwctype>
#include <array>
#include <vector>
#include <stack>
#include <random>

// OpenGL 4.6
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// OpenAL 1.1
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>

// Dear ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// nlohmann json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// boost-pool
#include <boost/pool/pool_alloc.hpp>

// EnTT
#include <entt.hpp>

// GLM
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
using namespace glm;

// For
#include <forcore.hpp>
#include <forarray.hpp>
#include <fortraits.hpp>
#include <forconvert.hpp>
#include <forfile.hpp>

// GLFW
#include "FOR_GLFW.h"

// Dear ImGui
#include "FOR_IMGUI.h"

// ECS
#include "Entity.h"
#include "ScriptableEntity.h"
#include "Components.h"

// Resource Management
#include "ResourceManager.h"

// Window
#include "Window.h"

// Scene Management
#include "Scene.h"
#include "SceneSerializer.h"

using namespace FE2D;