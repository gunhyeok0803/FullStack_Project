#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h> 
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") 

const std::string HOST = "127.0.0.1";
const int PORT = 65456;

// Create a separate receive handler
void recvHandler(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, 1024);
        int bytesReceived = recvfrom(clientSocket, buffer, 1024, 0, NULL, NULL);
        
        if (bytesReceived <= 0) {
            break;
        }

        std::string recvData(buffer);
        std::cout << "received: " << recvData << "\n";
        
        if (recvData == "quit") {
            break;
        }
    }
}

void main_client() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << "\n";
        return;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed." << "\n";
        WSACleanup();
        return;
    }

    // Server connection information
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST.c_str(), &serverAddr.sin_addr);

    std::thread clientThread(recvHandler, clientSocket);
    
    clientThread.detach();

    while (true) {
        // {start}
        std::string sendMsg;
        std::getline(std::cin, sendMsg);

        sendto(clientSocket, sendMsg.c_str(), sendMsg.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        
        if (sendMsg == "quit") {
            break;
        }
        // {end}
    }
    
    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    std::cout << "> echo-client is activated" << "\n";
    main_client();
    std::cout << "> echo-client is de-activated" << "\n";
    return 0;
}