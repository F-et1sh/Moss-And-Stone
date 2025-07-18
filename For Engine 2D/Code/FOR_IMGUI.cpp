#include "forpch.h"
#include "FOR_IMGUI.h"

#include "RenderContext.h"
#include "ProjectVariables.h"

#undef min
#undef max

void FE2D::IMGUI::Release() {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }
    
    m_Window = nullptr;
	m_ResourceManager = nullptr;
    m_RenderContext = nullptr;
}

void FE2D::IMGUI::Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager, ProjectVariables& project_variables) {
    Release();

    m_Window = &window;
    m_RenderContext = &render_context;
    m_ResourceManager = &resource_manager;
    m_ProjectVariables = &project_variables;
    
    m_Window->SubscribeToEvent(m_Event_WindowResized, EventType::WindowResized, [&](const IEvent& e) {
        m_CtrlWasPressed = false; // mouse position will be shifted after window resizing and you need reset values
        });

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Disable cursor changing

    // This is jgl_demos's style. Thank u bro, this colors are very beautiful
    auto& colors = ImGui::GetStyle().Colors;
    {
        colors[ImGuiCol_WindowBg          ] = ImVec4{ 0.10f, 0.10f, 0.10f, 1.0f };

        colors[ImGuiCol_Header            ] = ImVec4{ 0.20f, 0.20f, 0.20f, 1.0f };
        colors[ImGuiCol_HeaderHovered     ] = ImVec4{ 0.30f, 0.30f, 0.30f, 1.0f };
        colors[ImGuiCol_HeaderActive      ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

        colors[ImGuiCol_Button            ] = ImVec4{ 0.20f, 0.20f, 0.20f, 1.0f };
        colors[ImGuiCol_ButtonHovered     ] = ImVec4{ 0.30f, 0.30f, 0.30f, 1.0f };
        colors[ImGuiCol_ButtonActive      ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

        colors[ImGuiCol_FrameBg           ] = ImVec4{ 0.20f, 0.20f, 0.20f, 1.0f };
        colors[ImGuiCol_FrameBgHovered    ] = ImVec4{ 0.30f, 0.30f, 0.30f, 1.0f };
        colors[ImGuiCol_FrameBgActive     ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

        colors[ImGuiCol_Tab               ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
        colors[ImGuiCol_TabHovered        ] = ImVec4{ 0.38f, 0.38f, 0.38f, 1.0f };
        colors[ImGuiCol_TabActive         ] = ImVec4{ 0.28f, 0.28f, 0.28f, 1.0f };
        colors[ImGuiCol_TabUnfocused      ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.20f, 0.20f, 0.20f, 1.0f };

        colors[ImGuiCol_TitleBg           ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
        colors[ImGuiCol_TitleBgActive     ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed  ] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    }

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
	
	if (!ImGui_ImplGlfw_InitForOpenGL(m_Window->reference(), true))
		FOR_RUNTIME_ERROR("ERROR : Failed to Initialize ImGui\nImplGlfw_InitForOpenGL");
	
	if (!ImGui_ImplOpenGL3_Init(GLSL_VERSION))
        FOR_RUNTIME_ERROR("ERROR : Failed to Initialize ImGui\nImplOpenGL3_Init");

    const auto font_path = FOR_PATH.get_executable_path() / DEFAULT_FONT;
    try {
        const std::string string_path = FE2D::wstring_to_string(font_path.wstring());
        io.Fonts->AddFontFromFileTTF(string_path.c_str(), DEFAULT_PIXEL_SIZE);
    }
    catch (const std::exception& e) {
        SAY("ERROR : Failed to Load Font. Using Default Font" <<
            "\nFont Path : " << font_path <<
            "\nPixel Size : " << DEFAULT_PIXEL_SIZE <<
            "\nImGui : " << e.what());
    }
}

void FE2D::IMGUI::BeginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void FE2D::IMGUI::EndFrame() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void FE2D::IMGUI::StartDockSpace() {
    // Flags to make the window invisible
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
    {
        ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
}

void FE2D::IMGUI::CheckBox(const std::string& label, bool& value, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::Checkbox("##bool", &value);

    ImGui::Columns(1);
    ImGui::PopID();
}

void FE2D::IMGUI::DragInt(const std::string& label, int& value, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
    ImVec2 button_size = ImVec2(line_height + 3.0f, line_height);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::DragInt("##int", &value, 0.1f);

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

void FE2D::IMGUI::DragFloat(const std::string& label, float& value, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
    ImVec2 button_size = ImVec2(line_height + 3.0f, line_height);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::DragFloat("##float", &value, 0.1f);

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

void FE2D::IMGUI::DragVector2(const std::string& label, vec2& values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
    ImVec2 button_size = ImVec2(line_height + 3.0f, line_height);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    {
        if (ImGui::Button("X", button_size))
            values.x = resetValue;
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.5f);
    }
    {
        if (ImGui::Button("Y", button_size))
            values.y = resetValue;
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.5f);
    }

    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();
}

void FE2D::IMGUI::DragVector2I(const std::string& label, ivec2& values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
    ImVec2 button_size = ImVec2(line_height + 3.0f, line_height);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    {
        if (ImGui::Button("X", button_size))
            values.x = resetValue;
        ImGui::SameLine();
        ImGui::DragInt("##X", &values.x, 0.5f);
    }
    {
        if (ImGui::Button("Y", button_size))
            values.y = resetValue;
        ImGui::SameLine();
        ImGui::DragInt("##Y", &values.y, 0.5f);
    }

    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();
}

void FE2D::IMGUI::DragVector2_U64(const std::string& label, size_t& width, size_t& height, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    float line_height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
    ImVec2 button_size = ImVec2(line_height + 3.0f, line_height);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    {
        if (ImGui::Button("X", button_size))
            width = resetValue;
        ImGui::SameLine();
        ImGui::DragScalar("##X", ImGuiDataType_U64, &width, 0.5f, 0);
    }
    {
        if (ImGui::Button("Y", button_size))
            height = resetValue;
        ImGui::SameLine();
        ImGui::DragScalar("##Y", ImGuiDataType_U64, &height, 0.5f, 0);
    }

    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();
}

void FE2D::IMGUI::SelectTexture(const std::string& label, ResourceID<Texture>& id) {
    ImGui::Button((label + "##TEXTURE_SELECTION").c_str());

    constexpr ImVec2 texture_size = ImVec2(100, 100);
    constexpr float padding = 10.0f;
    
    const auto& resource_array = m_ResourceManager->getCache().get_resource_array();

    FE2D::UUID selected_uuid = id.uuid;

    static char search_buffer[128] = "";
    ImGui::Begin("Texture Selection");

    ImGui::InputTextWithHint("##Search", "Search texture...", search_buffer, IM_ARRAYSIZE(search_buffer));

    float window_visible_x = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    for (const auto& [uuid, resource] : resource_array) {
        if (auto* texture = dynamic_cast<Texture*>(resource)) {
            const std::string& name = m_ResourceManager->getCache().get_metadata(uuid).stem().string();

            if (strlen(search_buffer) > 0) {
                std::string name_lower = name;
                std::string search_lower = search_buffer;
                std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
                std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);

                if (name_lower.find(search_lower) == std::string::npos) continue;
            }

            ImGui::PushID(texture);

            static constexpr ImVec4 selected_bg_color = ImVec4(0, 1, 1, 0.5);
            static constexpr ImVec4 selected_tint_color = ImVec4(0, 0.5, 0.5, 1);

            ImVec4 bg_color = ImVec4(0, 0, 0, 0);
            ImVec4 tint_color = ImVec4(1, 1, 1, 1);

            if (uuid == selected_uuid) {
                bg_color = selected_bg_color;
                tint_color = selected_tint_color;
            }

            ImVec2 button_size = ImVec2(
                texture_size.x * (float(texture->getSize().x) / texture->getSize().y),
                texture_size.y
            );

            if (ImGui::ImageButton("##Texture", texture->reference(), button_size, ImVec2(0, 1), ImVec2(1, 0), bg_color, tint_color)) {
                selected_uuid = uuid;
                ImGui::PopID();
                break;
            }

            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("TEXTURE_UUID", &uuid, sizeof(FE2D::UUID));
                ImGui::Text("%s", name.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", name.c_str());

            float last_button_x = ImGui::GetItemRectMax().x;
            float next_button_x = last_button_x + padding + button_size.x;

            if (next_button_x < window_visible_x)
                ImGui::SameLine();

            ImGui::PopID();
        }
    }

    ImGui::End();

    id.uuid = selected_uuid;
}

void FE2D::IMGUI::SelectAnimation(ResourceID<Animation>& id) {
    const auto& resource_array = m_ResourceManager->getCache().get_resource_array();

    constexpr ImVec2 sprite_size = ImVec2(100, 100);
    constexpr float padding = 10.0f;

    FE2D::UUID selected_uuid = id.uuid;

    static char search_buffer[128] = "";
    ImGui::Begin("Animation Selection");

    ImGui::InputTextWithHint("##Search", "Search animation...", search_buffer, IM_ARRAYSIZE(search_buffer));

    float window_visible_x = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    for (const auto& [uuid, resource] : resource_array) {
        if (auto* animation = dynamic_cast<Animation*>(resource)) {
            const std::string& name = m_ResourceManager->getCache().get_metadata(uuid).stem().string();

            if (strlen(search_buffer) > 0) {
                std::string name_lower = name;
                std::string search_lower = search_buffer;
                std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
                std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);

                if (name_lower.find(search_lower) == std::string::npos) continue;
            }

            ResourceID<Texture> tex_id = animation->getTexture(*m_ResourceManager);
            auto& texture = m_ResourceManager->GetResource(tex_id);
            
            ImGui::PushID(animation);

            static constexpr ImVec4 selected_bg_color = ImVec4(1, 0.7f, 0.2f, 0.5f);
            static constexpr ImVec4 selected_tint_color = ImVec4(1, 0.6f, 0.2f, 1);

            ImVec4 bg_color = ImVec4(0, 0, 0, 0);
            ImVec4 tint_color = ImVec4(1, 1, 1, 1);

            if (uuid == selected_uuid) {
                bg_color = selected_bg_color;
                tint_color = selected_tint_color;
            }

            vec4 frame_uv = animation->getFrameUV(ImGui::GetTime());

            ImVec2 image_size = ImVec2(
                sprite_size.x * (float(frame_uv.z) / frame_uv.w),
                sprite_size.y
            );

            frame_uv /= vec4(texture.getSize(), texture.getSize());

            ImVec2 uv0(frame_uv.x, frame_uv.w + frame_uv.y);
            ImVec2 uv1(frame_uv.z + frame_uv.x, frame_uv.y);

            if (ImGui::ImageButton("##Animation", texture.reference(), image_size, uv0, uv1, bg_color, tint_color))
                selected_uuid = uuid;

            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("ANIMATION_UUID", &uuid, sizeof(FE2D::UUID));
                ImGui::Text("%s", name.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", name.c_str());

            float last_button_x = ImGui::GetItemRectMax().x;
            float next_button_x = last_button_x + padding + image_size.x;

            if (next_button_x < window_visible_x)
                ImGui::SameLine();

            ImGui::PopID();
        }
    }

    ImGui::End();

    id.uuid = selected_uuid;
}

void FE2D::IMGUI::SelectPrefab(ResourceID<Prefab>& id) {
    const auto& resource_array = m_ResourceManager->getCache().get_resource_array();

    std::string_view button_name = "Prefab";
    if (id.uuid != FE2D::UUID(0)) {
        auto& prefab = m_ResourceManager->GetResource(id);
        button_name = prefab.main_entity().GetComponent<TagComponent>().tag; // prefabs always has TagComponent
    }

    ImGui::Button(button_name.data());

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB_DRAGGING")) {
            ResourceID<Prefab> dropped_prefab_id = *static_cast<ResourceID<Prefab>*>(payload->Data);
            id = dropped_prefab_id;
        }
        ImGui::EndDragDropTarget();
    }
}

void FE2D::IMGUI::SelectLayer(const std::string& label, uint8_t& layer) {
    ImGui::Text(label.c_str());

    ImGui::SameLine();

    auto& physics_layers = m_ProjectVariables->getPhysicsLayers();

    std::string_view layer_button_name = "None##LAYER_CHOOSING";

    auto name = physics_layers.get_name_by_mask(layer);
    if (!name.empty()) layer_button_name = name;

    if (ImGui::Button(layer_button_name.data())) {
        ImGui::OpenPopup("##LAYER_CHOOSING");
    }

    if (ImGui::BeginPopup("##LAYER_CHOOSING")) {
        size_t i = 0;
        for (auto& info : physics_layers.getLayers()) {
            if (info.name.empty()) continue;
            if (ImGui::MenuItem((info.name + "##LAYER_" + std::to_string(i)).c_str())) 
                layer = i;
            i++;
        }

        ImGui::EndPopup();
    }
}

void FE2D::IMGUI::DrawAnimation(ResourceID<Animation> id, ImVec2 sprite_size) {
    auto& animation = m_ResourceManager->GetResource(id);
    auto texture_id = animation.getTexture(*m_ResourceManager);
    auto& texture = m_ResourceManager->GetResource(texture_id);

    vec4 frame_uv = animation.getFrameUV(ImGui::GetTime());

    ImVec2 image_size = ImVec2(
        sprite_size.x * (float(frame_uv.z) / frame_uv.w),
        sprite_size.y
    );

    frame_uv /= vec4(texture.getSize(), texture.getSize());

    ImVec2 uv0(frame_uv.x, frame_uv.w + frame_uv.y);
    ImVec2 uv1(frame_uv.z + frame_uv.x, frame_uv.y);

    ImGui::Image(texture.reference(), image_size, uv0, uv1);
}

void FE2D::IMGUI::DrawCollider(Entity entity) {
    if (!entity.HasComponent<TransformComponent>() || !entity.HasComponent<PhysicsComponent>()) return;

    auto& transform = entity.GetComponent<TransformComponent>();
    auto& physics = entity.GetComponent<PhysicsComponent>();

    mat4 matrix = entity.GetGlobalTransform();
    matrix = translate(matrix, vec3(physics.position, 0));

    constexpr ImU32 rect_color = IM_COL32(20, 240, 40, 100);

    vec2 top_left_world     = GetWorldPosition(matrix * translate(mat4(1.0f), vec3(-physics.size * vec2(0.5f, 0.5f), 0.0f)));
    vec2 bottom_right_world = GetWorldPosition(matrix * translate(mat4(1.0f), vec3( physics.size * vec2(0.5f, 0.5f), 0.0f)));

    ImVec2 rect_min(top_left_world.x, top_left_world.y);
    ImVec2 rect_max(bottom_right_world.x, bottom_right_world.y);

    std::swap(rect_min.y, rect_max.y);

    auto draw = this->GetPreviewWindowDrawList();
    draw->AddRectFilled(rect_min, rect_max, rect_color);
}

void FE2D::IMGUI::DrawTexture(ResourceID<Texture> id, ImVec2 sprite_size, ImVec2 uv0, ImVec2 uv1) {
    Texture& texture = m_ResourceManager->GetResource(id);
    ImVec2 button_size = ImVec2(
        sprite_size.x * (float(texture.getSize().x) / texture.getSize().y),
        sprite_size.y
    );
    ImGui::Image(texture.reference(), button_size, uv0, uv1);
}

vec2 FE2D::IMGUI::GetWorldPosition(const mat4& matrix) {
    mat4 mvp_matrix = m_RenderContext->getViewProjection() * matrix;
    vec4 clip_space_pos = mvp_matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec3 ndc_pos = vec3(clip_space_pos.x, clip_space_pos.y, clip_space_pos.z) / clip_space_pos.w;

    vec2 image_world_pos = vec2(m_PreviewImagePosition.x, (ImGui::GetIO().DisplaySize.y / 2) - m_PreviewImagePosition.y);

    vec2 pos = image_world_pos + vec2(
        ((ndc_pos.x + 1.0f) * 0.5f) * m_PreviewWindowSize.x,
        ((1.0f - ndc_pos.y) * 0.5f) * m_PreviewWindowSize.y
    );

    return pos;
}

vec2 FE2D::IMGUI::extractScale(const glm::mat4& matrix) {
    vec2 scale;
    scale.x = length(vec3(matrix[0]));
    scale.y = length(vec3(matrix[1]));
    return scale;
}

vec2 FE2D::IMGUI::extractPosition(const glm::mat4& matrix) {
    vec2 position;
    position.x = matrix[3][0];
    position.y = matrix[3][1];
    return position;
}

ImDrawList* FE2D::IMGUI::GetPreviewWindowDrawList() {
    vec4 clip = vec4(vec2(m_PreviewWindowPosition.x, (ImGui::GetIO().DisplaySize.y / 2) - m_PreviewImagePosition.y), m_PreviewWindowSize);
    ImVec2 clip_min = ImVec2(clip.x, clip.y);

    ImVec2 clip_max = ImVec2(clip.x + clip.z,
        clip.y + clip.w);

    ImDrawList* draw = ImGui::GetForegroundDrawList();
    draw->PushClipRect(clip_min, clip_max, true);

    return draw;
}

void IMGUI::TransformControl(Entity entity) {
    if (IsAnyColliderGizmoDragging()) return;

    auto& transform = entity.GetComponent<TransformComponent>();
    mat4 matrix = entity.GetGlobalTransform();

    m_IsAnyGizmoHovered = false;

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (m_IsDraggingX) m_IsDraggingX = false;
        if (m_IsDraggingY) m_IsDraggingY = false;
        if (m_IsDraggingRect) m_IsDraggingRect = false;
    }

    vec2 pos = this->GetWorldPosition(matrix);
    
    vec2 start = vec2(pos.x, pos.y);

    vec2 x_end = start + vec2(50, 0 );
    vec2 y_end = start - vec2(0 , 50);

    constexpr vec4 x_color = vec4(1.00f, 0.25f, 0.25f, 0.75f);
    constexpr vec4 y_color = vec4(0.25f, 0.25f, 1.00f, 0.75f);
    
    vec2 rect_start = start + vec2(1, 0); // this needed to fix a visual bug
    constexpr vec2 rect_size = vec2(15, -15);
    constexpr vec4 rect_color = vec4(0.15f, 0.75f, 0.15f, 0.75f);

    const float camera_zoom = m_RenderContext->getCamera()->getZoom();
    vec2 scale = vec2(1, 1);
    if (entity.HasParent())
        scale = extractScale(entity.GetParent().GetGlobalTransform());
    float viewport_scale_x = (LOGICAL_RESOLUTION.x / m_PreviewWindowSize.x) * (1.0f / camera_zoom) / scale.x;
    float viewport_scale_y = (LOGICAL_RESOLUTION.y / m_PreviewWindowSize.y) * (1.0f / camera_zoom) / scale.y;

    // step with pressed CTRL
    constexpr vec2 step_size = vec2(16);

    auto draw = this->GetPreviewWindowDrawList();

    if (!m_IsDraggingRect) {
        if (!m_IsDraggingY && DrawGizmoArrow(start, x_end, x_color, m_IsDraggingX, draw)) {
            vec2 delta = vec2(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y);

            if (ImGui::GetIO().KeyCtrl) {
                if (!m_CtrlWasPressed) {
                    m_CtrlWasPressed = true;
                    m_InitialMousePosition = vec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
                    m_InitialTransformPosition = transform.position;
                }

                vec2 mouseOffset = vec2(ImGui::GetIO().MousePos.x - m_InitialMousePosition.x, -(ImGui::GetIO().MousePos.y - m_InitialMousePosition.y));
                transform.position.x = m_InitialTransformPosition.x + round((mouseOffset.x * viewport_scale_x) / step_size.x) * step_size.x;
            }
            else {
                transform.position.x += delta.x * viewport_scale_x;
                m_CtrlWasPressed = false;
            }

            m_IsDraggingX = true;
        }
        if (!m_IsDraggingX && DrawGizmoArrow(start, y_end, y_color, m_IsDraggingY, draw)) {
            vec2 delta = vec2(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y);

            if (ImGui::GetIO().KeyCtrl) {
                if (!m_CtrlWasPressed) {
                    m_CtrlWasPressed = true;
                    m_InitialMousePosition = vec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
                    m_InitialTransformPosition = transform.position;
                }

                vec2 mouseOffset = vec2(ImGui::GetIO().MousePos.x - m_InitialMousePosition.x, -(ImGui::GetIO().MousePos.y - m_InitialMousePosition.y));
                transform.position.y = m_InitialTransformPosition.y + round((mouseOffset.y * viewport_scale_y) / step_size.y) * step_size.y;
            }
            else {
                transform.position.y += delta.y * viewport_scale_y;
                m_CtrlWasPressed = false;
            }

            m_IsDraggingY = true;
        }
    }
    if (!m_IsDraggingX && !m_IsDraggingY && DrawGizmoRectangle(rect_start, rect_size, rect_color, m_IsDraggingRect, draw)) {
        vec2 delta = vec2(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y);

        if (ImGui::GetIO().KeyCtrl) {
            if (!m_CtrlWasPressed) {
                m_CtrlWasPressed = true;
                m_InitialMousePosition = vec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
                m_InitialTransformPosition = transform.position;
            }

            vec2 mouseOffset = vec2(ImGui::GetIO().MousePos.x - m_InitialMousePosition.x, -(ImGui::GetIO().MousePos.y - m_InitialMousePosition.y));
            transform.position = m_InitialTransformPosition + round((mouseOffset * vec2(viewport_scale_x, viewport_scale_y)) / step_size) * step_size;
        }
        else {
            transform.position += delta * vec2(viewport_scale_x, viewport_scale_y);
            m_CtrlWasPressed = false;
        }

        m_IsDraggingRect = true;
    }
}

void FE2D::IMGUI::TilemapControl(Entity entity) {
    /*if (!entity.HasComponent<TransformComponent>() || !entity.HasComponent<TilemapComponent>()) return;

    auto& transform = entity.GetComponent<TransformComponent>();
    auto& tilemap = entity.GetComponent<TilemapComponent>();

    static FE2D::UUID last_entity = entity.GetUUID();
    static ResourceID<Texture> selected_texture;
    static uint8_t selected_tile;

    if (last_entity != entity.GetUUID()) {
        selected_texture = {};
        selected_tile = 0;

        last_entity = entity.GetUUID();
    }

    this->SelectTexture("Texture to create a tile", selected_texture);
    this->DrawTexture(selected_texture);

    if (ImGui::Button("Add 15-piece tileset")) {
        TilemapComponent::TileType type;
        type.mask = 1ull << tilemap.tile_types.size();
        type.texture_atlas = selected_texture;
        tilemap.tile_types.emplace_back(type);

        for (int x = 0; x < 64; x += 16) {
            for (int y = 0; y < 64; y += 16) {
                TilemapComponent::TileInfo tile_info;
                tile_info.frame = ivec4(x, y, 16, 16);
                tile_info.type = tilemap.tile_types.size() - 1;

                tilemap.tile_info.emplace_back(tile_info);
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Add Tile")) {
        TilemapComponent::TileType type;
        type.mask = 1ull << tilemap.tile_types.size();
        type.texture_atlas = selected_texture;
        tilemap.tile_types.emplace_back(type);

        Texture& texture = m_ResourceManager->GetResource(selected_texture);

        TilemapComponent::TileInfo tile_info;
        tile_info.frame = ivec4(0, 0, texture.getSize());
        tile_info.type = tilemap.tile_types.size() - 1;

        tilemap.tile_info.emplace_back(tile_info);
    }

    float window_visible_x = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    size_t i = 0;
    for (auto& info : tilemap.tile_info) {
        ImGui::PushID(i);

        ImVec2 uv0 = ImVec2(0, 1);
        ImVec2 uv1 = ImVec2(1, 0);

        Texture& texture = m_ResourceManager->GetResource(tilemap.tile_types[info.type].texture_atlas);

        uv0.x = info.frame.x / texture.getSize().x;
        uv0.y = info.frame.y / texture.getSize().y;

        uv1.x = uv0.x + (info.frame.z / texture.getSize().x);
        uv1.y = uv0.y + (info.frame.w / texture.getSize().y);

        std::swap(uv0.y, uv1.y);

        constexpr static ImVec2 button_size = ImVec2(100, 100);

        if (ImGui::ImageButton("##Tile", texture.reference(), button_size, uv0, uv1)) {
            selected_tile = i;
        }

        float last_button_x = ImGui::GetItemRectMax().x;
        float next_button_x = last_button_x + 10.0f + button_size.x;

        if (next_button_x < window_visible_x && i < tilemap.tile_info.size() - 1)
            ImGui::SameLine();
        
        ImGui::PopID();
        i++;
    }

    ImGui::Separator();

    if (tilemap.tile_types.empty())
        ImGui::Text("There is no tiles in this tilemap. Please create at least one");

    if (ImGui::BeginTable("TileInterectionsTable", tilemap.tile_types.size() + 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        for (auto& type : tilemap.tile_types) {
            ImGui::TableNextColumn();
            this->DrawTexture(type.texture_atlas);
        }
        ImGui::TableNextColumn();

        for (uint8_t row = 0; row < tilemap.tile_types.size(); row++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            this->DrawTexture(tilemap.tile_types[row].texture_atlas);

            for (uint8_t col = 0; col < tilemap.tile_types.size(); col++) {
                ImGui::TableNextColumn();
                if (col > row) continue;

                bool on = tilemap.is_compatible(row, col);
                std::string id = "##l" + std::to_string(row) + '_' + std::to_string(col);
                if (ImGui::Checkbox(id.c_str(), &on))
                    tilemap.set_compatible(row, col, on);
            }
        }
        ImGui::EndTable();
    }

    ImGui::Separator();

    if (tilemap.tiles.size() < tilemap.width * tilemap.height)
        tilemap.tiles.resize(tilemap.width * tilemap.height);

    constexpr vec2 cell_size = vec2(16.0f, 16.0f);
    constexpr ImColor cell_color = IM_COL32(80, 80, 80, 255);

    auto draw = this->GetPreviewWindowDrawList();

    for (size_t x = 0; x < tilemap.width; x++) {
        for (size_t y = 0; y < tilemap.height; y++) {
            vec2 offset = vec2(x, y) * cell_size;
            mat4 entity_matrix = entity.GetGlobalTransform();
            mat4 cell_matrix = entity_matrix * translate(mat4(1.0f), vec3(offset, 0.0f));

            vec2 top_left     = GetWorldPosition(cell_matrix * translate(mat4(1.0f), vec3(-cell_size * 0.5f, 0.0f)));
            vec2 bottom_right = GetWorldPosition(cell_matrix * translate(mat4(1.0f), vec3( cell_size * 0.5f, 0.0f)));

            ImVec2 rect_min = ImVec2(
                std::min(top_left.x, bottom_right.x),
                std::min(top_left.y, bottom_right.y)
            );
            ImVec2 rect_max = ImVec2(
                std::max(top_left.x, bottom_right.x),
                std::max(top_left.y, bottom_right.y)
            );

            draw->AddRect(rect_min, rect_max, cell_color);
            
            uint8_t& tile = tilemap.tiles[y * tilemap.width + x];

            if (ImGui::IsMouseDown(0)) {
                ImVec2 mouse_pos = ImGui::GetMousePos();
                if (mouse_pos.x >= rect_min.x && mouse_pos.x <= rect_max.x &&
                    mouse_pos.y >= rect_min.y && mouse_pos.y <= rect_max.y) {
                    tile = selected_tile;
                }
            }

            if (tile < tilemap.tile_info.size()) {
                auto& info = tilemap.tile_info[tile];

                Texture& texture = m_ResourceManager->GetResource(tilemap.tile_types[info.type].texture_atlas);

                ImVec2 uv0 = ImVec2(0, 1);
                ImVec2 uv1 = ImVec2(1, 0);

                uv0.x = info.frame.x / texture.getSize().x;
                uv0.y = info.frame.y / texture.getSize().y;

                uv1.x = uv0.x + (info.frame.z / texture.getSize().x);
                uv1.y = uv0.y + (info.frame.w / texture.getSize().y);

                std::swap(uv0.y, uv1.y);

                draw->AddImage(texture.reference(), rect_min, rect_max, uv0, uv1);
            }
        }
    }*/
}

bool FE2D::IMGUI::InputText(const std::string& label, std::string& value, int text_width) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strncpy_s(buffer, sizeof(buffer), value.c_str(), sizeof(buffer));

    ImGui::PushID(label.c_str());

    ImGui::Text(label.c_str());

    ImGui::SameLine();

    bool changed = false;

    ImGui::PushItemWidth(text_width);

    if (ImGui::InputText(("##input" + label).c_str(), buffer, sizeof(buffer))) {
        value = std::string(buffer);
        if (value.empty()) value = " ";
        changed = true;
    }

    m_IsAnyTextInput = ImGui::IsItemFocused();

    ImGui::PopID();

    return changed;
}

bool IMGUI::DrawGizmoArrow(const vec2& from, const vec2& to, const vec4& color, bool is_dragging, ImDrawList* draw, bool is_filled) {
    ImGuiIO& io = ImGui::GetIO();

    ImVec2 point_a(from.x, from.y);
    ImVec2 point_b(to.x, to.y);

    constexpr float thickness = 2.0f;
    ImVec2 minBB(std::min(point_a.x, point_b.x) - thickness, std::min(point_a.y, point_b.y) - thickness);
    ImVec2 maxBB(std::max(point_a.x, point_b.x) + thickness, std::max(point_a.y, point_b.y) + thickness);

    bool hovered = ImGui::IsMouseHoveringRect(minBB, maxBB, false);
    if (hovered) m_IsAnyGizmoHovered = true;

    bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

    ImU32 arrow_color = 0;

    const ImU32 drag_color  = IM_COL32(color.r * 255, color.g * 255, color.b *   5, color.a * 255);
    const ImU32 hover_color = IM_COL32(color.r * 200, color.g * 200, color.b * 200, color.a * 255);
    const ImU32 base_color  = IM_COL32(color.r * 255, color.g * 255, color.b * 255, color.a * 255);

    if (is_dragging)  arrow_color = drag_color;
    else if (hovered) arrow_color = hover_color;
    else              arrow_color = base_color;

    draw->AddLine(point_a, point_b, arrow_color);
    
    vec2 dir = normalize(to - from);
    vec2 perp = vec2(-dir.y, dir.x);
    
    constexpr float size = 12.0f;
    
    ImVec2 tip1 = ImVec2(point_b.x - dir.x * size + perp.x * size * 0.5f, point_b.y - dir.y * size + perp.y * size * 0.5f);
    ImVec2 tip2 = ImVec2(point_b.x - dir.x * size - perp.x * size * 0.5f, point_b.y - dir.y * size - perp.y * size * 0.5f);
    is_filled ? draw->AddTriangleFilled(point_b, tip1, tip2, arrow_color) : draw->AddTriangle(point_b, tip1, tip2, arrow_color);
    
    if (is_dragging) return true;

    return held;
}

bool IMGUI::DrawGizmoRectangle(const vec2& position, const vec2& size, const vec4& color, bool is_dragging, ImDrawList* draw, bool is_filled) {
    ImGuiIO& io = ImGui::GetIO();

    ImVec2 rect_min(position.x, position.y);
    ImVec2 rect_max(position.x + size.x, position.y + size.y);
    
    const vec2 thickness = abs(size);
    ImVec2 minBB(rect_min.x - thickness.x, rect_min.y - thickness.y);
    ImVec2 maxBB(rect_max.x + thickness.x, rect_max.y + thickness.y);

    bool hovered = ImGui::IsMouseHoveringRect(minBB, maxBB, false);
    if (hovered) m_IsAnyGizmoHovered = true;

    bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

    ImU32 rect_color = 0;

    const ImU32 drag_color  = IM_COL32(color.r * 255, color.g * 255, color.b *   5, color.a * 255);
    const ImU32 hover_color = IM_COL32(color.r * 200, color.g * 200, color.b * 200, color.a * 255);
    const ImU32 base_color  = IM_COL32(color.r * 255, color.g * 255, color.b * 255, color.a * 255);

    if (is_dragging)  rect_color = drag_color;
    else if (hovered) rect_color = hover_color;
    else              rect_color = base_color;

    is_filled ? draw->AddRectFilled(rect_min, rect_max, rect_color) : draw->AddRect(rect_min, rect_max, rect_color);

    if (is_dragging) return true;

    return held;
}