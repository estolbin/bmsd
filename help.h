#ifndef HELP_H_INCLUDED
#define HELP_H_INCLUDED

LRESULT CALLBACK HELPProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rc;
    switch (msg)
    {
        case WM_CREATE:
            GetClientRect(hWnd, &rc);
            return 0;
        // HDC hdc, hdccomp;
        // HBITMAP hbitmap;
        // PAINTSTRUCT ps;
        // RECT rc;
        // GetClientRect(hWnd, &rc);
        // BITMAP bitmap;
    case WM_PAINT:
        {
            const wchar_t* title = L"Application guide";
            hdc = BeginPaint(hWnd, &ps);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            TextOut(hdc, 100, 0, title, lstrlen(title));
            EndPaint(hWnd, &ps);
        }
        return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

#endif // HELP_H_INCLUDED
