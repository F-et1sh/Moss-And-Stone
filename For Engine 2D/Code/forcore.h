#pragma once

namespace FE2D {

#ifdef _WIN32
    #ifdef FORENGINE2D_EXPORTS
        #define FOR_API __declspec(dllexport)
    #else
        #define FOR_API __declspec(dllimport)
    #endif
#else
    #define FOR_API
#endif

#define FOR_NODISCARD [[nodiscard]]

#define FOR_STRINGIFY(x) #x
#define FOR_TOSTRING(x) FE2D::string_to_wstring(FOR_STRINGIFY(x))

#define SAY(message) std::wcerr << std::endl << message << std::endl
#define SAY_withoutBreak(message) std::wcerr << message

#define FOR_RUNTIME_ERROR(message) throw std::runtime_error("RUNTIME ERROR : " + std::string(message))

#define FOR_ASSERT(exp, ...) if (!(exp)) { SAY(L"CRITICAL ERROR : " << __VA_ARGS__); __debugbreak(); }

    // Soon
    /*
    * #pragma region Soon
    #include <Windows.h>

    #define HR(x) __hr(x, __FILE__, __LINE__)
    #define safe_delete(x) if (x) { delete x; x = nullptr; }
    #define safe_release(x) if (x) { x->Release(); x = nullptr; }

    inline static void __hr(HRESULT hr, LPCSTR filename, int line) {
        if (FAILED(hr)) {
            char* buffer = {};
            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&buffer, 0, NULL
            );
            LocalFree(buffer);
            char totalCuffer[256];
            filename = (::strrchr(filename, '\\') + 1);
            sprintf_s(totalCuffer, "%s\nfile: %s\n line %d\n", buffer, filename, line);

            if (MessageBoxA(NULL, totalCuffer, "HRESULT Error", MB_OK)) {
                ExitProcess(0);
            }
        }
    }
    #pragma endregion
    */
}