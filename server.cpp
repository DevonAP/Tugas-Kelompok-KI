#include <iostream>
#include "DES.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
    const string key = "81384935"; // Kunci DES 8 karakter
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(9999);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);
    cout << "Menunggu koneksi..." << endl;

    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int clientSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
    cout << "Terhubung!" << endl;

    char buffer[1024];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Pesan: " << buffer << endl;

    const char* reply = "Pesan diterima!";
    des_encrypt(reply, key);
    send(clientSocket, reply, strlen(reply), 0);

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}
