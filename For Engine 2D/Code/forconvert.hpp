#pragma once
#include <string>
#include <Windows.h>

namespace FE2D {
    inline static constexpr std::string wstring_to_string(const std::wstring& load_wstring) {
        if (load_wstring.empty()) return "";

        int len = WideCharToMultiByte(CP_UTF8, 0, load_wstring.c_str(), -1, NULL, 0, NULL, NULL);
        if (len < 0) return "";

        std::string load_path(len - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, load_wstring.c_str(), -1, &load_path[0], len, NULL, NULL);
        return load_path;
    }

    inline static constexpr std::wstring string_to_wstring(const std::string& load_string) {
        if (load_string.empty()) return L"";

        int len = MultiByteToWideChar(CP_UTF8, 0, load_string.c_str(), -1, NULL, 0);
        if (len < 0) return L"";
            
        std::wstring load_wstring(len - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, load_string.c_str(), -1, &load_wstring[0], len);
        return load_wstring;
    }
}