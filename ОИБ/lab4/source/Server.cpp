#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include <sstream>
#include <string>
#include <winsock2.h>

using namespace std;

int Server()
{
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

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    do {
        newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
    } while (newConnection == 0);

    std::cout << "Client connected\n" << std::endl;
    char command[256];
    do {
        cin.getline(command, 256);
        if (send(newConnection, command, sizeof(command), NULL) < 0)
        {
            std::cout << "Error sending command\n" << std::endl;
            return 3;
        }
    } while (strcmp(command, "exit") != 0);
}

int main()
{
    Server();
}