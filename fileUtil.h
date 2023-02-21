#include <windows.h>

#pragma once

std::vector<std::string> g_fileList;

void SaveRecentFilesToRegistry(const std::vector<std::string>& recentFiles);
std::vector<std::string> LoadRecentFilesFromRegistry();
LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
