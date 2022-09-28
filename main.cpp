#define UNICODE

#include <Windows.h>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <time.h>
#include <sstream>
#include <gdiplus.h>
using namespace Gdiplus;

using namespace std;

#include "definitions.h"
#include "resource.h"
#include "Connection.h"
#include "menus_and_widgets.h"
#include "plots.h"
#include "help.h"

unsigned char OWI_COMPUTECRC8(unsigned char inData, unsigned char seed) // CRC computing function
{
    unsigned char bitsLeft;
    unsigned char temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        inData >>= 1;
    }
    return seed;
}


unsigned char getSeed(unsigned char* line)
{
    unsigned char seed = 0;

    /* 1, 2, 3 bytes */
    for (int i = 1; i < 4; i++)
    {
        seed = OWI_COMPUTECRC8(line[i], seed);
    }

    return seed;
}


void CenterWindow(HWND hwnd) // MainWindow centering function
{
    RECT rc = {0};

    GetWindowRect(hwnd, &rc);
    int win_w = rc.right - rc.left;
    int win_h = rc.bottom - rc.top;

    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);

    SetWindowPos(hwnd, HWND_TOP, (screen_w - win_w)/2,
                 (screen_h - win_h)/2, 0, 0, SWP_NOSIZE);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    SoftwareMainClass.style		    = CS_HREDRAW | CS_VREDRAW;
    SoftwareMainClass.lpfnWndProc   = SoftwareMainProcedure;
    SoftwareMainClass.hInstance	    = hInst;
    SoftwareMainClass.hIcon		    = LoadIcon (hInst, MAKEINTRESOURCE(IDI_ICON2));
    SoftwareMainClass.hCursor	    = LoadCursor (NULL, IDC_ARROW);
    SoftwareMainClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    SoftwareMainClass.lpszMenuName  = NULL;
    SoftwareMainClass.lpszClassName = L"Main";

    if (!RegisterClassW(&SoftwareMainClass))
    {
        return -1;
    }

    // Register class for help window
    SoftwareMainClass.lpfnWndProc    = HELPProc;
    SoftwareMainClass.hIcon		  = NULL;
    SoftwareMainClass.lpszMenuName  = NULL;
    SoftwareMainClass.lpszClassName = L"help";
    RegisterClass(&SoftwareMainClass);

    // Register class for draw plot window
    SoftwareMainClass.lpfnWndProc    = DrawProcedure;
    SoftwareMainClass.hIcon		  = NULL;
    SoftwareMainClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    SoftwareMainClass.lpszMenuName  = NULL;
    SoftwareMainClass.lpszClassName = L"draw";
    RegisterClass(&SoftwareMainClass);

    // Register class for open plot window
    SoftwareMainClass.lpfnWndProc    = GraphProcedure;
    SoftwareMainClass.hIcon		  = NULL;
    SoftwareMainClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    SoftwareMainClass.lpszMenuName  = NULL;
    SoftwareMainClass.lpszClassName = L"graphics";
    RegisterClass(&SoftwareMainClass);

    MSG SoftwareMainMessage = { 0 };

    hwnd_0 = CreateWindow(L"Main", L"NavLab", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 380, NULL, NULL, hInst, NULL);

    ShowWindow(hwnd_0, ncmdshow);
    UpdateWindow(hwnd_0);

    while (GetMessage(&SoftwareMainMessage, NULL, 0, 0))
    {
        TranslateMessage(&SoftwareMainMessage);
        DispatchMessage(&SoftwareMainMessage);
    }

    TerminateThread(readThread, 0); //???
    return 0;
}


void ExitSoftware(void)
{
    isConnected = false;
    isThreading = false;
    CloseHandle(connectedPort);
    CloseHandle(readThread);
    TerminateThread(readThread, 0); //???
    PostQuitMessage(0);
}


void LoadData(LPCSTR path, HWND control)
{
    HANDLE FileToLoad = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD bytesIterated;
    ReadFile(FileToLoad, Buffer, TextBufferSize, &bytesIterated, NULL);

    SetWindowTextA(control, Buffer);

    CloseHandle(FileToLoad);
}


void SetOpenFileParams(HWND hwnd_0)
{
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd_0;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = NULL;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}


LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_COMMAND:

        if ((wp >= ComSelectIndex) && (wp < ComSelectIndex + ComPortAmount))
        {
            selectedPort = wp - ComSelectIndex;
            SetWindowStatus("PORT: " + std::to_string(selectedPort));
            SerialUpdate();
            break;
        }
        switch (wp)
        {
        case draw_plot:

            gr_draw = CreateWindow(L"draw", L"DRAW", WS_VISIBLE | WS_BORDER | WS_MAXIMIZE | WS_OVERLAPPEDWINDOW, 0, 0, 800, 700, NULL, NULL, hInst, NULL);

            break;
        case open_plot:

            gr_open = CreateWindow(L"graphics", L"OPEN", WS_VISIBLE | WS_BORDER | WS_MAXIMIZE | WS_OVERLAPPEDWINDOW, 0, 0, 800, 700, NULL, NULL, hInst, NULL);

            break;
        case OnConnectRequest:
            ConnectRequest();
            break;
        case emergency_stop_button:
            SerialWriteSTOP();
            break;
        case send_commands:
            SerialWriteTEST();
            break;
        case open_file:
            if (GetOpenFileNameA(&ofn))
            {
                LoadData(filename, hEditControl);
            }
            break;
        case OnSerialRefresh:
            SerialUpdate();
            break;
        case OnExitSoftware:
            PostQuitMessage(0);
            break;
        case help:

            help_win = CreateWindow(L"help", L"HELP", WS_VISIBLE | WS_BORDER | WS_MAXIMIZE | WS_HSCROLL | WS_VSCROLL | WS_OVERLAPPEDWINDOW, 0, 0, 800, 700, hWnd, NULL, hInst, NULL);

            break;
        default:
            break;
        }
        break;

    case WM_SIZE:
    {
        RECT rc{};
        GetClientRect(hWnd, &rc);
        SetWindowPos(hStaticControl, NULL, 0, 10, rc.right, 20, SWP_NOZORDER);
        SetWindowPos(hEditControl, NULL, 40, 40 + rc.bottom / 8, rc.right-80, rc.bottom / 2, SWP_NOZORDER);
        SetWindowPos(open_file_but, NULL, rc.right / 10, 40, rc.right / 3, rc.bottom / 9.5, SWP_NOZORDER);
        SetWindowPos(send_but, NULL, rc.right / 10 + rc.right / 3 + rc.right * 2/15, 40, rc.right / 3, rc.bottom / 9.5, SWP_NOZORDER);
        SetWindowPos(emergency_stop_but, NULL, 0, 50 + rc.bottom / 8 + rc.bottom / 2, rc.right, rc.bottom / 4.9, SWP_NOZORDER);
    }
    break;
    case WM_CREATE:
    {
        MainWndAddMenus(hWnd);
        MainWndAddWidgets(hWnd);
        SerialUpdate();
        SetOpenFileParams(hWnd);
        readThread = CreateThread(NULL, 0, SerialRead, NULL, 0, NULL);
        CenterWindow(hWnd);
    }
    break;
    case WM_DESTROY: // close mainwindow
        ExitSoftware();
        break;
    default:
        return DefWindowProc(hWnd, msg, wp, lp);
    }
}


void SetWindowStatus(std::string status)
{
    SetWindowTextA(hStaticControl, status.c_str());
}
