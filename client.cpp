#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.70.1"); // IP lokal server

    connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

    const char* msg = "Halo dari client!";
    send(sock, msg, strlen(msg), 0);

    char buffer[1024];
    recv(sock, buffer, sizeof(buffer), 0);
    cout << "Balasan server: " << buffer << endl;

    closesocket(sock);
    WSACleanup();
}
