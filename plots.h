#ifndef PLOTS_H_INCLUDED
#define PLOTS_H_INCLUDED


LRESULT CALLBACK DrawProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    int i;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    GetClientRect (hWnd, &rect); // client window size

    switch (msg)
    {

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        const int xWS = rect.right; // horizontal working window size
        const int yWS = rect.bottom; // vertical working window size

        const int OmegaGridStep = 2; // Omega_step
        const int OmegaMin = -30;  // Omega_min
        const int OmegaMax = 30;  // Omega_max

        int tMin = 0;
        int tMax_1 = 60;

        SetViewportOrgEx(hdc, 0, yWS / 2, NULL); // origin point

        double nPixPerOmega = yWS / 60;
        double nPixPerT = xWS / 60;

        int yMin = OmegaMin * nPixPerOmega;
        int yMax = OmegaMax * nPixPerOmega;

        const double tGridStep_1_min = 1;
        int xMin = tMin * nPixPerT;
        int xMax_1 = tMax_1 * nPixPerT;

        // OX
        HPEN hPen1=CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
        SelectObject(hdc, hPen1);

        MoveToEx(hdc, 20, 0, NULL);
        LineTo(hdc, xMax_1 + 20, 0);

        TextOutA(hdc, xWS - xWS / 20, yWS / 50, "T,min", 6);

        // OY
        MoveToEx(hdc, 20, yMin, NULL);
        LineTo(hdc, 20, yMax);

        TextOutA(hdc, 15, -yWS / 2, "Omega,deg/sec", 14);

        // grid
        HPEN hPen0=CreatePen(PS_SOLID, 1, RGB(0, 160, 0));
        HPEN hOldPen=(HPEN)SelectObject(hdc, hPen0);

        int vertical_grid_0 = OmegaMin;
        int horizontal_grid_0 = tMin;
        int x, y;
        UINT uAlignPrev;
        for (int i=0; i<(sizeof(yMark)/sizeof(yMark[0])); ++i) // vertical
        {
            y = vertical_grid_0 * nPixPerOmega;
            MoveToEx(hdc, xMin + 20, y, NULL);
            LineTo(hdc, xMax_1 + 20, y);
            vertical_grid_0 += OmegaGridStep;
            static LOGFONT lf;
            lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
            lf.lfItalic = TRUE;
            lf.lfHeight = 12;
            SetTextColor(hdc, RGB(0, 0, 200));
            uAlignPrev = SetTextAlign(hdc, TA_CENTER);
            HFONT hFont0=CreateFontIndirect(&lf);
            HFONT hOldFont=(HFONT) SelectObject(hdc, hFont0);

            TextOutA(hdc, xMin+10, y - 7, yMark[i], strlen(yMark[i]));
        }
        for (int j=1; j<(sizeof(xMark_1)/sizeof(xMark_1[0])); ++j) // horizontal
        {
            x = horizontal_grid_0 * nPixPerT;
            MoveToEx(hdc, x + 20, yMin, NULL);
            LineTo(hdc, x + 20, yMax);
            horizontal_grid_0 += tGridStep_1_min;
            TextOutA(hdc, x+40, 3, xMark_1[j], strlen(xMark_1[j]));
        }
        EndPaint(hWnd, &ps);
    }
    case WM_LBUTTONDOWN:
    {
        HDC hdc;
        WORD nSize, xPos, yPos;

        TCHAR szBuf[80];
        DWORD dw;
        HANDLE File;
        RECT rc;
        GetClientRect (hWnd, &rc); // client window size

        const int xW = rc.right;
        const int yW = rc.bottom;
        int y_draw_point;
        // click coordinates
        xPos = LOWORD(lp);
        yPos = HIWORD(lp);
        hdc = GetDC(hWnd);

        if (xPos > 0 & yPos > 0 & xPos < xW & yPos < yW)
        {
            HDC hDC;

            hDC=BeginPaint(hWnd, &ps);
            hDC=GetDC(hWnd);
            HBRUSH hBrush;
            hBrush=CreateSolidBrush(RGB(255,0,0));
            SelectObject(hDC,hBrush);
            Ellipse(hDC, xPos - 4, yPos - 4, xPos + 4, yPos + 4);
            //MoveToEx(hDC, 20, yW / 2, NULL);
            //LineTo(hDC, xPos, yPos);
            EndPaint(hWnd, &ps);
        }
        nSize = wsprintf(szBuf, TEXT("%d %d\r\n"), xPos, yPos);
        TextOut(hdc, xPos, yPos, szBuf, nSize);
        ReleaseDC(hWnd, hdc);

        File = CreateFile(TEXT("draw_points.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (File == INVALID_HANDLE_VALUE)
        {
            return 1;
        }

        SetFilePointer(File, 0, 0, FILE_END);
        WriteFile(File, &szBuf, nSize*sizeof(TCHAR), &dw, NULL);

        CloseHandle(File);
        return 0;
    }
    }
    return DefWindowProc(hWnd, msg, wp, lp);

}


void LoadData_gr(LPCSTR path)
{
    HANDLE FileToLoad = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD bytesIterated;
    ReadFile(FileToLoad, Buffer, TextBufferSize, &bytesIterated, NULL);

    SetWindowTextA(hEditPoints, Buffer);

    CloseHandle(FileToLoad);
}


LRESULT CALLBACK GraphProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    int i;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    GetClientRect (hWnd, &rect); // client window size
    float nScale = 1;

    switch (msg)
    {

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        std::ifstream omega_file_sum("test.txt");
        int Omega;
        double T, T_sum = 0;
        while (omega_file_sum >> T >> Omega)
        {
            T_sum += T;
        }

        const int xWS = rect.right; // horizontal working window size
        const int yWS = rect.bottom; // vertical working window size

        // vertical separator
        HPEN hPenSep=CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
        HPEN hOldPenSep=(HPEN)SelectObject(hdc, hPenSep);
        MoveToEx(hdc, xWS - xWS / 6, - yWS, NULL);
        LineTo(hdc, xWS - xWS / 6, yWS);

        const int OmegaGridStep = 2; // Omega_step
        const int OmegaMin = -30;  // Omega_min
        const int OmegaMax = 30;  // Omega_max

        int tMin = 0;
        int tMax_1 = 60;
        int tMax_2 = 60 * 2;
        int tMax_5 = 60 * 5;
        int tMax_10 = 60 * 10;
        int tMax_30 = 60 * 30;

        SetViewportOrgEx(hdc, 0, yWS / 2, NULL); // origin point

        double nPixPerOmega = yWS / 60;
        double nPixPerT = xWS / 74;

        int yMin = OmegaMin * nPixPerOmega;
        int yMax = OmegaMax * nPixPerOmega;

        if (T_sum > tMin & T_sum <= tMax_1)
        {
            const double tGridStep_1_sec = 1;
            int xMin = tMin * nPixPerT;
            int xMax_1 = tMax_1 * nPixPerT;

            // OX
            HPEN hPen1=CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
            SelectObject(hdc, hPen1);

            MoveToEx(hdc, 20, 0, NULL);
            LineTo(hdc, xMax_1 + 20, 0);

            TextOutA(hdc, xWS - xWS / 6 - xWS / 26, yWS / 50, "T,sec", 6);

            // OY
            MoveToEx(hdc, 20, yMin, NULL);
            LineTo(hdc, 20, yMax);

            TextOutA(hdc, 15, -yWS / 2, "Omega,deg/sec", 14);

            // grid
            HPEN hPen0=CreatePen(PS_SOLID, 1, RGB(0, 160, 0));
            HPEN hOldPen=(HPEN)SelectObject(hdc, hPen0);

            int vertical_grid_0 = OmegaMin;
            int horizontal_grid_0 = tMin;
            int x, y;
            UINT uAlignPrev;
            for (int i=0; i<(sizeof(yMark)/sizeof(yMark[0])); ++i) // vertical
            {
                y = vertical_grid_0 * nPixPerOmega;
                MoveToEx(hdc, xMin + 20, y, NULL);
                LineTo(hdc, xMax_1 + 20, y);
                vertical_grid_0 += OmegaGridStep;
                static LOGFONT lf;
                lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
                lf.lfItalic = TRUE;
                lf.lfHeight = 12;
                SetTextColor(hdc, RGB(0, 0, 200));
                uAlignPrev = SetTextAlign(hdc, TA_CENTER);
                HFONT hFont0=CreateFontIndirect(&lf);
                HFONT hOldFont=(HFONT) SelectObject(hdc, hFont0);

                TextOutA(hdc, xMin+10, y - 7, yMark[i], strlen(yMark[i]));
            }
            for (int j=1; j<(sizeof(xMark_1)/sizeof(xMark_1[0])); ++j) // horizontal
            {
                x = horizontal_grid_0 * nPixPerT;
                MoveToEx(hdc, x + 20, yMin, NULL);
                LineTo(hdc, x + 20, yMax);
                horizontal_grid_0 += tGridStep_1_sec;
                TextOutA(hdc, x+40, 3, xMark_1[j], strlen(xMark_1[j]));
            }

            // plot
            double vertical_step_grid = (yMax - yMin) / 60;
            double horizontal_step_grid = (xMax_1 - xMin) / (60);

            HPEN hPenPlot=CreatePen(PS_SOLID, 3, RGB(0, 0, 250));
            HPEN hOldPenPlot=(HPEN)SelectObject(hdc, hPenPlot);

            double T_new = 0;
            double currentY = 0;

            std::ifstream omega_file("test.txt");
            MoveToEx(hdc, 20, 0, NULL); // start point

            while (omega_file >> T >> Omega)
            {
                double nextY = - Omega * vertical_step_grid;
                LineTo(hdc, 20 + (T_new) * horizontal_step_grid * nScale, currentY);
                LineTo(hdc, 20 + (T_new) * horizontal_step_grid * nScale, nextY);
                currentY = nextY;

                T_new = T_new + T;
                LineTo(hdc, 20 + (T_new) * horizontal_step_grid, nextY);
            }
        }
        else if (T_sum > tMax_1 & T_sum <= tMax_2)
        {
            const double tGridStep_2_sec = 1;
            int xMin = tMin * nPixPerT;
            int xMax_2 = tMax_1 * nPixPerT;

            // OX
            HPEN hPen1=CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
            SelectObject(hdc, hPen1);

            MoveToEx(hdc, 20, 0, NULL);
            LineTo(hdc, xMax_2 + 20, 0);

            TextOutA(hdc, xWS - xWS / 6 - xWS / 26, yWS / 50, "T,sec", 6);

            // OY
            MoveToEx(hdc, 20, yMin, NULL);
            LineTo(hdc, 20, yMax);

            TextOutA(hdc, 15, -yWS / 2, "Omega,deg/sec", 14);

            // grid
            HPEN hPen0=CreatePen(PS_SOLID, 1, RGB(0, 160, 0));
            HPEN hOldPen=(HPEN)SelectObject(hdc, hPen0);

            int vertical_grid_0 = OmegaMin;
            int horizontal_grid_0 = tMin;
            int x, y;
            UINT uAlignPrev;
            for (int i=0; i<(sizeof(yMark)/sizeof(yMark[0])); ++i) // vertical
            {
                y = vertical_grid_0 * nPixPerOmega;
                MoveToEx(hdc, xMin + 20, y, NULL);
                LineTo(hdc, xMax_2 + 20, y);
                vertical_grid_0 += OmegaGridStep;
                static LOGFONT lf;
                lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
                lf.lfItalic = TRUE;
                lf.lfHeight = 12;
                SetTextColor(hdc, RGB(0, 0, 200));
                uAlignPrev = SetTextAlign(hdc, TA_CENTER);
                HFONT hFont0=CreateFontIndirect(&lf);
                HFONT hOldFont=(HFONT) SelectObject(hdc, hFont0);

                TextOutA(hdc, xMin+10, y - 7, yMark[i], strlen(yMark[i]));
            }
            for (int j=1; j<(sizeof(xMark_2)/sizeof(xMark_2[0])); ++j) // horizontal
            {
                x = horizontal_grid_0 * nPixPerT;
                MoveToEx(hdc, x + 20, yMin, NULL);
                LineTo(hdc, x + 20, yMax);
                horizontal_grid_0 += tGridStep_2_sec;
                TextOutA(hdc, x+40, 3, xMark_2[j], strlen(xMark_2[j]));
            }

            // plot
            double vertical_step_grid = (yMax - yMin) / 60;
            double horizontal_step_grid = (xMax_2 - xMin) / (60 * 2);

            HPEN hPenPlot=CreatePen(PS_SOLID, 3, RGB(0, 0, 250));
            HPEN hOldPenPlot=(HPEN)SelectObject(hdc, hPenPlot);

            double T_new = 0;
            double currentY = 0;

            std::ifstream omega_file("test.txt");
            MoveToEx(hdc, 20, 0, NULL); // start point

            while (omega_file >> T >> Omega)
            {
                double nextY = - Omega * vertical_step_grid;
                LineTo(hdc, 20 + T_new * horizontal_step_grid, currentY);
                LineTo(hdc, 20 + T_new * horizontal_step_grid, nextY);
                currentY = nextY;

                T_new = T_new + T;
                LineTo(hdc, 20 + T_new * horizontal_step_grid, nextY);
            }
        }
        else if (T_sum > tMax_30 & T_sum <= tMax_30 * 2)
        {
            const double tGridStep_1_min = 1;
            int xMin = tMin * nPixPerT;
            int xMax_1 = tMax_1 * nPixPerT;

            // OX
            HPEN hPen1=CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
            SelectObject(hdc, hPen1);

            MoveToEx(hdc, 20, 0, NULL);
            LineTo(hdc, xMax_1 + 20, 0);

            TextOutA(hdc, xWS - xWS / 6 - xWS / 26, yWS / 50, "T,min", 6);

            // OY
            MoveToEx(hdc, 20, yMin, NULL);
            LineTo(hdc, 20, yMax);

            TextOutA(hdc, 15, -yWS / 2, "Omega,deg/sec", 14);

            // grid
            HPEN hPen0=CreatePen(PS_SOLID, 1, RGB(0, 160, 0));
            HPEN hOldPen=(HPEN)SelectObject(hdc, hPen0);

            int vertical_grid_0 = OmegaMin;
            int horizontal_grid_0 = tMin;
            int x, y;
            UINT uAlignPrev;

            for (int i=0; i<(sizeof(yMark)/sizeof(yMark[0])); ++i) // vertical
            {
                y = vertical_grid_0 * nPixPerOmega;
                MoveToEx(hdc, xMin + 20, y, NULL);
                LineTo(hdc, xMax_1 + 20, y);
                vertical_grid_0 += OmegaGridStep;
                static LOGFONT lf;
                lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
                lf.lfItalic = TRUE;
                lf.lfHeight = 12;
                SetTextColor(hdc, RGB(0, 0, 200));
                uAlignPrev = SetTextAlign(hdc, TA_CENTER);
                HFONT hFont0=CreateFontIndirect(&lf);
                HFONT hOldFont=(HFONT) SelectObject(hdc, hFont0);

                TextOutA(hdc, xMin+10, y - 7, yMark[i], strlen(yMark[i]));
            }
            for (int j=1; j<(sizeof(xMark_1)/sizeof(xMark_1[0])); ++j) // horizontal
            {
                x = horizontal_grid_0 * nPixPerT;
                MoveToEx(hdc, x + 20, yMin, NULL);
                LineTo(hdc, x + 20, yMax);
                horizontal_grid_0 += tGridStep_1_min;
                TextOutA(hdc, x+40, 3, xMark_1[j], strlen(xMark_1[j]));
            }

            // plot
            double vertical_step_grid = (yMax - yMin) / 60;
            double horizontal_step_grid = (xMax_1 - xMin) / (60);

            HPEN hPenPlot=CreatePen(PS_SOLID, 3, RGB(0, 0, 250));
            HPEN hOldPenPlot=(HPEN)SelectObject(hdc, hPenPlot);

            double T_new = 0;
            double currentY = 0;

            std::ifstream omega_file("test.txt");
            MoveToEx(hdc, 20, 0, NULL); // start point

            while (omega_file >> T >> Omega)
            {
                double nextY = - Omega * vertical_step_grid;
                LineTo(hdc, 20 + (T_new / 60) * horizontal_step_grid * nScale, currentY);
                LineTo(hdc, 20 + (T_new / 60) * horizontal_step_grid * nScale, nextY);
                currentY = nextY;

                T_new = T_new + T;
                LineTo(hdc, 20 + (T_new / 60) * horizontal_step_grid, nextY);
            }
        }

        EndPaint(hWnd, &ps);
    }
    break;
    case open_file_gr:
            if (GetOpenFileNameA(&ofn))
            {
                LoadData_gr(filename);
            }
    case WM_CREATE:
        GraphicsWidgets(hWnd);
        MainWndAddMenus(hWnd);
        SetOpenFileParams_gr(hWnd);
        break;
    case WM_SIZE:
        SetWindowPos(open_file_gr_but, NULL, rect.right - rect.right / 6 + 10, 10, rect.right - 5 * rect.right / 6 - 20, 40, SWP_NOZORDER);
        SetWindowPos(hEditPoints, NULL, rect.right - rect.right / 6 + 10, 60, rect.right - 5 * rect.right / 6 - 20, 80, SWP_NOZORDER);
        SetWindowPos(flip_plot_but, NULL, rect.right - rect.right / 6 + 10, 150, rect.right - 5 * rect.right / 6 - 20, 40, SWP_NOZORDER);
        break;

    }
    return DefWindowProc(hWnd, msg, wp, lp);
}


#endif // PLOTS_H_INCLUDED
