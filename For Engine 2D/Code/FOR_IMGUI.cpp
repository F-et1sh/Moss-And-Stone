#include "forpch.h"
#include "FOR_IMGUI.h"

#include "RenderContext.h"

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

void FE2D::IMGUI::Initialize(Window& window, RenderContext& render_context, ResourceManager& resource_manager) {
    Release();

    m_Window = &window;
    m_RenderContext = &render_context;
    m_ResourceManager = &resource_manager;
    
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

void FE2D::IMGUI::SelectTexture(ResourceID<Texture>& id) {
    const auto& resource_array = m_ResourceManager->getCache().get_resource_array();

    constexpr ImVec2 texture_size = ImVec2(100, 100);
    constexpr float padding = 10.0f;

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

                if (name_lower.find(search_lower) == std::string::npos)
                    continue;
            }

            ImGui::PushID(texture);

            ImVec2 button_size = ImVec2(
                texture_size.x * (float(texture->getSize().x) / texture->getSize().y),
                texture_size.y
            );

            static constexpr ImVec4 selected_bg_color = ImVec4(0, 1, 1, 0.5);
            static constexpr ImVec4 selected_tint_color = ImVec4(0, 0.5, 0.5, 1);

            ImVec4 bg_color = ImVec4(0, 0, 0, 0);
            ImVec4 tint_color = ImVec4(1, 1, 1, 1);

            if (uuid == selected_uuid) {
                bg_color = selected_bg_color;
                tint_color = selected_tint_color;
            }

            if (ImGui::ImageButton("##Texture", texture->reference(), button_size, ImVec2(0, 1), ImVec2(1, 0), bg_color, tint_color))
                selected_uuid = uuid;

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

                if (name_lower.find(search_lower) == std::string::npos)
                    continue;
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

void FE2D::IMGUI::DrawAnimation(ResourceID<Animation> id, ImVec2 sprite_size) {
    if (id.uuid == FE2D::UUID(0)) return;

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
    if (!m_IsDraggingX && !m_IsDraggingY && DrawGizmoRect(rect_start, rect_size, rect_color, m_IsDraggingRect, draw)) {
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

void FE2D::IMGUI::ColliderControl(TransformComponent& transform, ColliderComponent& collider) {
    if (IsAnyTransformGizmoDragging()) return;

    m_IsAnyGizmoHovered = false;

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (m_IsDraggingColliderLeft) m_IsDraggingColliderLeft = false;
		if (m_IsDraggingColliderRight) m_IsDraggingColliderRight = false;
		if (m_IsDraggingColliderTop) m_IsDraggingColliderTop = false;
		if (m_IsDraggingColliderBottom) m_IsDraggingColliderBottom = false;
    }

    vec2 pos = this->GetWorldPosition(transform);
    pos += collider.position;
    
    constexpr float line_width = 3;

    vec2 left_pos = pos;

	vec2 vertical_line_size   = vec2(line_width, collider.size.y);
	vec2 horizontal_line_size = vec2(collider.size.x, line_width);

    auto draw = this->GetPreviewWindowDrawList();

    constexpr vec4 color = vec4(0.4f, 0.7f, 0.5f, 1.0f);

    const float camera_zoom = m_RenderContext->getCamera()->getZoom();
    vec2 viewport_scale = (LOGICAL_RESOLUTION / m_PreviewWindowSize) * camera_zoom;

    // left
    if (DrawGizmoRect(left_pos, vertical_line_size, color, m_IsDraggingColliderLeft, draw)) {
        vec2 delta = vec2(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y);

        collider.position.x += delta.x * viewport_scale.x;

        m_IsDraggingColliderLeft = true;
    }
}

bool IMGUI::DrawGizmoArrow(const vec2& from, const vec2& to, const vec4& color, bool is_dragging, ImDrawList* draw) {
    ImGuiIO& io = ImGui::GetIO();

    ImVec2 point_a(from.x, from.y);
    ImVec2 point_b(to.x, to.y);

    constexpr float thickness = 2.0f;
    ImVec2 minBB(min(point_a.x, point_b.x) - thickness, min(point_a.y, point_b.y) - thickness);
    ImVec2 maxBB(max(point_a.x, point_b.x) + thickness, max(point_a.y, point_b.y) + thickness);

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
    draw->AddTriangleFilled(point_b, tip1, tip2, arrow_color);
    
    if (is_dragging)
        return true;

    return held;
}

bool IMGUI::DrawGizmoRect(const vec2& position, const vec2& size, const vec4& color, bool is_dragging, ImDrawList* draw) {
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

    draw->AddRectFilled(rect_min, rect_max, rect_color);

    if (is_dragging)
        return true;

    return held;
}