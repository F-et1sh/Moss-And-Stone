#pragma once
#include <Windows.h>
#include <commdlg.h>
#include <string>
#include <shlobj.h>
#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <cstdarg>

namespace FE2D {
    inline static const std::wstring OpenFileDialog() {
        wchar_t currentDir[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, currentDir);

        OPENFILENAME ofn;
        wchar_t szFileName[MAX_PATH] = L"";

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

        if (GetOpenFileName(&ofn)) {
            SetCurrentDirectory(currentDir);
            return szFileName;
        }
        else {
            SetCurrentDirectory(currentDir);
            return L"";
        }
    }
    inline static const std::wstring OpenFileDialog(std::wstring file_filter) {
        OPENFILENAME ofn;
        wchar_t szFileName[MAX_PATH] = L"";

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = file_filter.c_str();
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

        if (GetOpenFileName(&ofn)) return szFileName;
        else return L"";
    }
    inline static const std::wstring ChooseFolder() {
        BROWSEINFO bi = { 0 };
        bi.ulFlags = BIF_RETURNONLYFSDIRS;
        LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

        if (pidl != nullptr) {
            wchar_t path[MAX_PATH];
            if (SHGetPathFromIDList(pidl, path)) {
                IMalloc* imalloc = 0;
                if (SUCCEEDED(SHGetMalloc(&imalloc))) {
                    imalloc->Free(pidl);
                    imalloc->Release();
                }
                return path;
            }
            IMalloc* imalloc = 0;
            if (SUCCEEDED(SHGetMalloc(&imalloc))) {
                imalloc->Free(pidl);
                imalloc->Release();
            }
        }
        return L"";
    }

    namespace fs = std::filesystem;
    inline static void copy_file(const fs::path& from, const fs::path& to) {
        if (!fs::exists(to)) {
            fs::create_directories(to);
        }
        for (const auto& entry : fs::recursive_directory_iterator(from)) {
            const auto& destPath = to / fs::relative(entry.path(), from);
            if (fs::is_directory(entry.status())) {
                fs::create_directories(destPath);
            }
            else if (fs::is_regular_file(entry.status())) {
                fs::copy_file(entry.path(), destPath, fs::copy_options::overwrite_existing);
            }
        }
    }
    inline static void copy_if_new(const std::filesystem::path& from, const std::filesystem::path& to) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(from)) {
            auto relative_path = std::filesystem::relative(entry.path(), from);
            auto target_path = to / relative_path;

            if (entry.is_directory()) {
                std::filesystem::create_directories(target_path);
            }
            else if (!std::filesystem::exists(target_path) ||
                std::filesystem::last_write_time(entry) > std::filesystem::last_write_time(target_path)) {
                std::filesystem::copy_file(entry, target_path, std::filesystem::copy_options::overwrite_existing);
            }
        }
    }

    inline static std::filesystem::path generate_unique_filename(const std::filesystem::path& file_path) {
        std::filesystem::path path = file_path;
        std::filesystem::path stem = path.stem();
        std::filesystem::path extension = path.extension();
        int count = 1;

        while (std::filesystem::exists(path)) {
            path.replace_filename(stem.wstring() + L" (" + std::to_wstring(count) + L")" + extension.wstring());
            count++;
        }

        return path;
    }
}