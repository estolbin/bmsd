#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

int SerialBegin(int BaudRate, int Comport)
{
    CloseHandle(connectedPort);

    connectedPort = CreateFileA(("\\\\.\\COM" + std::to_string(Comport)).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);

    if (connectedPort == INVALID_HANDLE_VALUE)
    {
        return -4;    //No Port
    }

    DCB SerialParams;
    SerialParams.DCBlength = sizeof(SerialParams);
    if (!GetCommState(connectedPort, &SerialParams))
    {
        return -3;    //GetState error
    }

    SerialParams.BaudRate = BaudRate;
    SerialParams.ByteSize = 8;
    SerialParams.StopBits = ONESTOPBIT;
    SerialParams.Parity = NOPARITY;
    if (!SetCommState(connectedPort, &SerialParams))
    {
        return -2;    //SetState error
    }

    COMMTIMEOUTS SerialTimeouts;
    SerialTimeouts.ReadIntervalTimeout = ReadTimeout;
    SerialTimeouts.ReadTotalTimeoutConstant = 1;
    SerialTimeouts.ReadTotalTimeoutMultiplier = 1;
    SerialTimeouts.WriteTotalTimeoutConstant = 1;
    SerialTimeouts.WriteTotalTimeoutMultiplier = 1;
    if (!SetCommTimeouts(connectedPort, &SerialTimeouts))
    {
        return -1;    //SetTimeouts error
    }
    return 0;
}

void ConnectRequest(void)
{
    if (isConnected)
    {
        CloseHandle(connectedPort);
        SetWindowStatus("Disconnected");
        isConnected = false;
        return;
    }

    switch (SerialBegin(targetBaudRate, selectedPort))
    {
    case -4:
        SetWindowStatus("Device not connected!");
        break;
    case -3:
        SetWindowStatus("GetState error!");
        break;
    case -2:
        SetWindowStatus("SetState error!");
        break;
    case -1:
        SetWindowStatus("SetTimeouts error!");
        break;
    case 0:
        SetWindowStatus("Connected to: COM" + std::to_string(selectedPort));
        isConnected = true;
        return;
    }

    CloseHandle(connectedPort);
}

DWORD WINAPI SerialRead(LPVOID lpParameter)
{
    DWORD BytesIterated;

    while (isThreading)
    {
        if (!isConnected)
        {
            continue;
        }
        if (!SetCommMask(connectedPort, EV_RXCHAR))
        {
            ConnectRequest();
            continue;
        }
    }

    return 0;
}


bool send_cmd(unsigned char* cmd)
{
    DWORD n;

    // send command
    WriteFile(connectedPort, (LPCVOID)cmd, 5, (LPDWORD)(&n), NULL) || n < 5;

    return n == 5;
}


void SerialWriteTEST(void)
{
    double Omega;
    double T;

    char dir = 1;

    std::stringstream ss(Buffer);

    if (!isConnected)
    {
        return;
    }

    unsigned char everyCmd[] = { 0xE6, 0x00, 0x51, 0x00, 0xB3 };

    unsigned char dirPositive[5] = { 0xE6, 0x00, 0xA7, 0x00, 0x80 }; // dir positive
    unsigned char dirNegative[5] = { 0xE6, 0x00, 0xA7, 0x01, 0xDE }; // dir negative

    unsigned char
    init[][5] =
    {
        { 0xE6, 0x00, 0xA5, 0x18, 0x4E }, // max acceleration
        { 0xE6, 0x00, 0xA6, 0x18, 0x1B }, // max deceleration

        { 0xE6, 0x00, 0xA3, 0x00, 0xBB }, // 0
        { 0xE6, 0x00, 0x51, 0x00, 0xB3 }
    };

    for (int i = 0; i < sizeof(init) / sizeof(init[0]); i++)
    {
        if (!send_cmd(init[i]))
        {
            return;
        }

        if (i == 1)
        {
            if (!send_cmd(dirPositive))
            {
                return;
            }
        }
    }

    while (!ss.eof())
    {
        ss >> T >> Omega;

        if (Omega < 0 && dir > 0)
        {
            if (!send_cmd(dirNegative))
            {
                return;
            }

            if (!send_cmd(everyCmd))
            {
                return;
            }

            dir = -1;
        }
        else if (Omega > 0 && dir < 0)
        {
            if (!send_cmd(dirPositive))
            {
                return;
            }

            if (!send_cmd(everyCmd))
            {
                return;
            }

            dir = 1;
        }

        Omega = abs(Omega);
        U_0 = round(U_min + (Omega / Omega_) * (U_max - U_min));
        unsigned char cmd[5] = { 0xE6, 0x00, 0xA3, U_0 };

        cmd[4] = getSeed(cmd);

        if (!send_cmd(cmd))
        {
            return;
        }

        if (!send_cmd(everyCmd))
        {
            return;
        }

        Sleep(T * 1000);
    }
}


void SerialWriteSTOP()

{
    if (!isConnected)
    {
        return;
    }

    DWORD i, n, m;

    unsigned char
    cmd[][5] =
    {
        { 0xE6, 0x00, 0xA3, 0x00, 0xBB }, // 0
        { 0xE6, 0x00, 0x51, 0x00, 0xB3 },
        { 0xE6, 0x00, 0x52, 0x18, 0xB9 },
    };


    m = sizeof(cmd)/sizeof(cmd[0]);
    for (i = 0; i < m; i++)
    {
        // send command
        if (FALSE == WriteFile(connectedPort, (LPCVOID)cmd[i], sizeof(cmd[i]), (LPDWORD)(&n), NULL) || n < sizeof(cmd[i]))
        {
            printf("ERROR sending data");
            break;
        }
        Sleep(100);
    }

}


void SerialUpdate()
{
    while (RemoveMenu(ComPortListMenu, 0, MF_BYPOSITION));
    int radioLast = 0, radioCurrent = -1;

    for (int i = 1; i < ComPortAmount; ++i)
    {
        HANDLE port = CreateFileA(("\\\\.\\COM" + std::to_string(i)).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (port != INVALID_HANDLE_VALUE)
        {
            AppendMenuA(ComPortListMenu, MF_STRING, ComSelectIndex + i, ("COM" + std::to_string(i)).c_str());
            if (i == selectedPort)
            {
                radioCurrent = radioLast;
            }
            ++radioLast;
        }
        CloseHandle(port);
    }
    if (radioCurrent != -1)
    {
        CheckMenuItem(ComPortListMenu, radioCurrent, MF_BYPOSITION | MF_CHECKED);
    }
}
#endif // CONNECTION_H_INCLUDED
