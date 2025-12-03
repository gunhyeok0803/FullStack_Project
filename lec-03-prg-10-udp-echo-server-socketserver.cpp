#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class MyUDPHandler {
public:
    void handle(SOCKET serverSocket, std::string data, sockaddr_in clientAddr, int clientAddrLen) {
        // {start}
        std::string RecvData = data; 
        
        std::cout << "received: " << RecvData << "\n";
        std::cout << "echoed: " << RecvData << "\n";
        
        sendto(serverSocket, RecvData.c_str(), RecvData.length(), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
        // {end}
    }
};

int main() {
    const std::string HOST = "127.0.0.1";
    const int PORT = 65456;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed" << "\n";
        return 1;
    }

    std::cout << "echo-server is activated" << "\n";

    // Create the server 
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed" << "\n";
        WSACleanup();
        return 1;
    }

    // binding
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST.c_str(), &serverAddr.sin_addr);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Bind failed" << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    MyUDPHandler handler;

    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // Activate the server
    while (true) {
        memset(buffer, 0, 1024);
        
        // receive
        int bytesReceived = recvfrom(serverSocket, buffer, 1024, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        
        if (bytesReceived > 0) {
            std::string data(buffer, bytesReceived);
            
            handler.handle(serverSocket, data, clientAddr, clientAddrLen);
        }
    }

    std::cout << "echo-server is de-activated" << "\n";

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}