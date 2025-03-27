#pragma once
#include <iostream>
#include <string>

namespace FE2D {
	#define SAY(message) std::wcerr << std::endl << message << std::endl
	#define SAY_withoutBreak(message) std::wcerr << message

    #define FOR_RUNTIME_ERROR(message) throw std::runtime_error("RUNTIME ERROR : " + std::string(message))
	
	// Assert about a Critical Error
	#define ASSERT(exp) if(!exp) { std::wcout << L"Critical Error in File : " << __FILE__ << std::endl; throw; }

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