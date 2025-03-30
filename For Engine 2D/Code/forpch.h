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

// For
#include <forcore.h>
#include <formath.h>
#include <forarray.h>
#include <fordebug.h>
#include <fordebuggui.h>
#include <forsettings.h>
#include <forconvert.h>
#include <forfile.h>

// GLFW
#include "FOR_GLFW.h"

// Dear ImGui
#include "FOR_IMGUI.h"

// ECS
#include "ComponentRegistrar.h"

// Resource Management
#include "ResourceManager.h"
#include "PathManager.h"

// Render Managment
#include "RenderManager.h"
#include "RendererRegistrar.h"

using namespace FE2D;