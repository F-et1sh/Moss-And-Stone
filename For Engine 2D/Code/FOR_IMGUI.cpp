#include "forpch.h"
#include "FOR_IMGUI.h"

void FE2D::IMGUI::Release() {
    if (ImGui::GetCurrentContext()) {

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }
    m_IsInitialized = false;
}

void FE2D::IMGUI::setResourceManager(ResourceManager* resource_manager) noexcept {
    if (!resource_manager)
        FOR_RUNTIME_ERROR("Failed to Initialize ImGui\nResource Manager reference was nullptr");
    m_ResourceManager = resource_manager;
}

void FE2D::IMGUI::Initialize(GLFWwindow* window_reference) {
    Release();

    if (!window_reference)
        FOR_RUNTIME_ERROR("Failed to Initialize ImGui\nWindow reference was nullptr");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    const auto font_path = FOR_PATH.get_assets_path() / "Fonts" / "open_sans.ttf";
    try { 
        io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), DEFAULT_PIXEL_SIZE);
    }
    catch (const std::exception& e) {
        SAY("ERROR : Failed to Load Font. Using Default Font" <<
            "\nFont Path : " << font_path <<
            "\nPixel Size : " << DEFAULT_PIXEL_SIZE <<
            "\nImGui : " << e.what());
    }

    // The style is stealed from jgl_demos. Thank u bro, that's nice !!
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
	
	if (!ImGui_ImplGlfw_InitForOpenGL(window_reference, true))
		FOR_RUNTIME_ERROR("ERROR : Failed to Initialize ImGui\nImplGlfw_InitForOpenGL");
	
	if (!ImGui_ImplOpenGL3_Init(GLSL_VERSION))
        FOR_RUNTIME_ERROR("ERROR : Failed to Initialize ImGui\nImplOpenGL3_Init");

    m_IsInitialized = true;
}

inline void FE2D::IMGUI::BeginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

inline void FE2D::IMGUI::EndFrame() {
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

inline void FE2D::IMGUI::StartDockSpace() {
    // Flags to make the window invisible
    ImGuiWindowFlags windowFlags =
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

void FE2D::IMGUI::SelectTexture(const std::string& label, size_t& index) {
    try {
        const size_t texture_hash_code = typeid(Texture).hash_code();
        const auto& resource_array = m_ResourceManager->m_ResourceCache.get_resource_array().at(texture_hash_code);

        constexpr int item_size = 64;

        ImGui::Begin(label.c_str());

        int columnCount = int(ImGui::GetContentRegionAvail().x / item_size);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        for (const auto& resource : resource_array) {

            size_t res_index = resource.first;
            Texture* texture = static_cast<Texture*>(resource.second);

            if (!texture->is_Initialized())
                continue;

            // selectable image of the texture
            ImGui::ImageButton(std::to_string(res_index).c_str(),
                texture->reference(),
                { item_size, item_size }, ImVec2(0, 1), ImVec2(1, 0));

            // click to select the resource
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                index = res_index; // set the index
            }

            ImGui::NextColumn();
        }

        ImGui::Columns(1);

        ImGui::End();

        Texture* texture = static_cast<Texture*>(resource_array.at(index));
        if (texture) {
            ImGui::Image(texture->reference(), { item_size, item_size }, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    catch (const std::exception& e) {
        SAY("ERROR : Failed to select a texture in FOR_IMGUI\nException : " << e.what());
    }
}