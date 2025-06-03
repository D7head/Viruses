#include <Windows.h>
#include <cmath>
#include <ctime>

HHOOK hMouseHook, hKeyboardHook;

const int MELT_SPEED = 5;
const int FLIP_INTERVAL_MS = 3000;

COLORREF RandomColor() {
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

void InvertScreen(HDC hdc) {
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBmp = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    SelectObject(hdcMem, hBmp);
    BitBlt(hdcMem, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), hdc, 0, 0, SRCCOPY);
    StretchBlt(hdc, 0, GetSystemMetrics(SM_CYSCREEN), GetSystemMetrics(SM_CXSCREEN), -GetSystemMetrics(SM_CYSCREEN),
        hdcMem, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SRCCOPY);
    DeleteDC(hdcMem);
    DeleteObject(hBmp);
}

void MeltScreen(HDC hdc) {
    for (int y = GetSystemMetrics(SM_CYSCREEN) - 1; y > 0; y--) {
        for (int x = 0; x < GetSystemMetrics(SM_CXSCREEN); x += MELT_SPEED) {
            COLORREF color = GetPixel(hdc, x, y);
            SetPixel(hdc, x + (rand() % MELT_SPEED), y + (rand() % 2), color);
        }
    }
}

LRESULT CALLBACK ChaosLoop(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        HDC hdc = GetDC(NULL);
        for (int i = 0; i < 1000; i++) {
            SetPixel(hdc, rand() % GetSystemMetrics(SM_CXSCREEN),
                rand() % GetSystemMetrics(SM_CYSCREEN), RandomColor());
        }

        static DWORD lastFlip = GetTickCount();
        if (GetTickCount() - lastFlip > FLIP_INTERVAL_MS) {
            InvertScreen(hdc);
            lastFlip = GetTickCount();
        }

        MeltScreen(hdc);
        ReleaseDC(NULL, hdc);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(static_cast<unsigned>(time(nullptr)));

    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, ChaosLoop, hInstance, 0);
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, ChaosLoop, hInstance, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hMouseHook);
    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}
