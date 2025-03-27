#include "forpch.h"
#include "fordebuggui.h"

#include <stdarg.h>
#include "Renderer.h"
#include "Font.h"

//////////////////////////
#pragma region Variables

//Renderer d_Renderer;

Font d_Font;
//std::vector<Text> d_Texts;
unsigned int d_CurrentText = 0;

#pragma endregion
//////////////////////////
#pragma region Functions

FOR_API void FE2D_GUI::Release() {
	//d_Renderer.Release_Rectangles();
	//d_Renderer.Release_Texts();

	d_Font.Release();
}

FOR_API bool FE2D_GUI::Initialize() {
	//if (!d_Renderer.Initialize_Rectangles()) return false;
	//if (!d_Renderer.Initialize_Texts()) return false;

    d_Font.PrepareSymbols(0, 300);
    if (!d_Font.BuildFont("Files/Fonts/antonio_variable.ttf", vec2(4096, 4096), vec2(256), true)) return false;
}

FOR_API void FE2D_GUI::ShowInfo(const std::wstring fmt, ...) {
    va_list args;
    va_start(args, fmt);

    std::wstring buffer = std::wstring();
    auto i = fmt.begin();

    unsigned int strides_count = 1;

    while (i != fmt.end()) {
        if (*i == L'\n') strides_count++;
        if (*i == L'%') {
            i++;
            switch (*i) {
            case L'i': {
                int value = va_arg(args, int);
                buffer += std::to_wstring(value) + L" ";
                break;
            }
            case L'd': {
                double value = va_arg(args, double);
                buffer += std::to_wstring(value) + L" ";
                break;
            }
            case L'c': {
                wchar_t value = va_arg(args, wchar_t);
                buffer += std::to_wstring(value) + L" ";
                break;
            }
            case L's': {
                wchar_t* str = va_arg(args, wchar_t*);
                buffer += std::wstring(str) + L" ";
                break;
            }
            default:
                SAY(L"ERROR : Unknown Type - " << *i);
                break;
            }
        }
        else buffer += *i;
        i++;
    }

    va_end(args);

    //if (d_Texts.size() <= d_CurrentText) d_Texts.push_back(Text());

    //d_Texts[d_CurrentText].m_Color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    //d_Texts[d_CurrentText].m_Position = 
        //vec2(-SIZE_X / 2 + 15, 
              //SIZE_Y / 2 - 15 - (50 * d_CurrentText * strides_count));
    //d_Texts[d_CurrentText].m_Center = Text::Center::LEFT_TOP;
    //d_Texts[d_CurrentText].m_Layer = 1000.0f;
    //d_Texts[d_CurrentText].m_Scale = 0.5f;
    //d_Texts[d_CurrentText].m_String = buffer;

    d_CurrentText++;
}

FOR_API void FE2D_GUI::ShowInfo_Texture(std::wstring fmt, unsigned int load_texture) {
    mat4 load_matrix = mat4(1.0f);
    load_matrix *= translate(vec3(0, 0, 0));
    //load_matrix *= scale(vec3(Application::getAspect().x*2, Application::getAspect().y*2, 0));
    //d_Renderer.RenderRectangle(load_matrix, load_texture, vec4(1.0f, 1.0f, 1.0f, 1.0f));
    //d_OffsetY -= 200;
}

FOR_API void FE2D_GUI::End_of_Frame() {
    //d_Renderer.reset_Camera();
    //for (auto& text : d_Texts) {
        //d_Renderer.RenderText(text, d_Font);
        //text.m_String = L"";
    //}
    d_CurrentText = 0;
}

#pragma endregion
//////////////////////////