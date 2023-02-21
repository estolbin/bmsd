#include "fileUtil.h"

void SaveRecentFilesToRegistry(const std::vector<std::string>& recentFiles) {
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyApp", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        MessageBox(hwnd_9)
        return;
    }

    std::string value;
    for (int i = 0; i < recentFiles.size(); i++) {
        value += recentFiles[i] + '\0';
    }

    if (RegSetValueEx(hKey, "RecentFiles", 0, REG_MULTI_SZ, reinterpret_cast<const BYTE*>(value.c_str()), value.length()) != ERROR_SUCCESS) {
        // Обработка ошибки
        RegCloseKey(hKey);
        return;
    }

    RegCloseKey(hKey);
}


std::vector<std::string> LoadRecentFilesFromRegistry() {
    std::vector<std::string> recentFiles;

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\MyApp", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        // Обработка ошибки
        return recentFiles;
    }

    DWORD dataSize = 0;
    if (RegQueryValueEx(hKey, "RecentFiles", nullptr, nullptr, nullptr, &dataSize) != ERROR_SUCCESS) {
        // Обработка ошибки
        RegCloseKey(hKey);
        return recentFiles;
    }

    char* data = new char[dataSize];
    if (RegQueryValueEx(hKey, "RecentFiles", nullptr, nullptr, reinterpret_cast<BYTE*>(data), &dataSize) != ERROR_SUCCESS) {
        // Обработка ошибки
        RegCloseKey(hKey);
        delete[] data;
        return recentFiles;
    }

    int i = 0;
    while (i < dataSize) {
        std::string fileName = &data[i];
        recentFiles.push_back(fileName);
        i += fileName.length() + 1;
    }

    RegCloseKey(hKey);
    delete[] data;
    return recentFiles;
}

LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch(uMsg)
    {
        case WM_LBUTTONDOWN:
        {
            const auto result=DefSubclassProc(hWnd, uMsg, wParam, lParam);
 
            [&]
            {
                RECT r;
                GetWindowRect(hWnd, &r);
                POINT pt = {0, 0};
                ClientToScreen(hWnd, &pt);
                pt.y += (r.bottom - r.top);
 
                HMENU PopupMenu = CreatePopupMenu();
                AppendMenuW(PopupMenu, MF_STRING, 0, L"First");
                AppendMenuW(PopupMenu, MF_STRING, 1, L"Second");
                AppendMenuW(PopupMenu, MF_STRING, 2, L"Third");
                TrackPopupMenu(PopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
                DestroyMenu(PopupMenu);
            }();
 
            return result;
 
        }break;
 
        case WM_NCDESTROY:
        {
            RemoveWindowSubclass(hWnd, ButtonSubclassProc, uIdSubclass);
            
        }break;
    }
 
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}