#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

std::vector<HWND> g_icons;
volatile bool g_running = true;
std::mt19937 g_rng(std::random_device{}());

COLORREF HsvToRgb(float H, float S, float V) {
    float C = V * S;
    float X = C * (1.0f - fabs(fmod(H / 60.0f, 2.0f) - 1.0f));
    float m = V - C;

    float r, g, b;
    if (H >= 0 && H < 60) { r = C; g = X; b = 0; }
    else if (H >= 60 && H < 120) { r = X; g = C; b = 0; }
    else if (H >= 120 && H < 180) { r = 0; g = C; b = X; }
    else if (H >= 180 && H < 240) { r = 0; g = X; b = C; }
    else if (H >= 240 && H < 300) { r = X; g = 0; b = C; }
    else { r = C; g = 0; b = X; }

    return RGB(
        (int)((r + m) * 255),
        (int)((g + m) * 255),
        (int)((b + m) * 255)
    );
}

HWND CreateChaosIcon(int x, int y, COLORREF color) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"CHAOS_ICON";
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"CHAOS_ICON",
        L"",
        WS_POPUP,
        x, y, 64, 64,
        NULL, NULL, wc.hInstance, NULL
    );

    if (hwnd) {
        HDC hdc = GetDC(hwnd);
        HBITMAP hBmp = CreateCompatibleBitmap(hdc, 64, 64);
        HDC hMemDC = CreateCompatibleDC(hdc);
        SelectObject(hMemDC, hBmp);

        HBRUSH hBrush = CreateSolidBrush(color);
        SelectObject(hMemDC, hBrush);
        Ellipse(hMemDC, 0, 0, 64, 64);

        ICONINFO iconInfo = { TRUE, 0, 0, hBmp, hBmp };
        HICON hIcon = CreateIconIndirect(&iconInfo);

        SendMessage(hwnd, STM_SETICON, (WPARAM)hIcon, 0);
        SetLayeredWindowAttributes(hwnd, 0, 220, LWA_ALPHA);

        DeleteObject(hBrush);
        DeleteObject(hBmp);
        DeleteDC(hMemDC);
        ReleaseDC(hwnd, hdc);
    }
    return hwnd;
}

DWORD WINAPI ChaosEngine(LPVOID lpParam) {
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);
    std::uniform_int_distribution<int> distX(0, screenX - 64);
    std::uniform_int_distribution<int> distY(0, screenY - 64);

    while (g_running) {
        float hue = (float)(g_rng() % 360);
        COLORREF color = HsvToRgb(hue, 1.0f, 1.0f);
        HWND icon = CreateChaosIcon(distX(g_rng), distY(g_rng), color);

        if (icon) {
            g_icons.push_back(icon);
        }
        Sleep(100 + (g_rng() % 400));
    }
    return 0;
}

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow
) {
    HANDLE thread = CreateThread(NULL, 0, ChaosEngine, NULL, 0, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_running = false;
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    return 0;
}
