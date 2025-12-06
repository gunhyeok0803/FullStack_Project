#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h> 
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") 

const std::string HOST = "127.0.0.1";
const int PORT = 65456;

// receive handler
void recvHandler(SOCKET clientSocket) {
    char buffer[1024];
    sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);

    std::cout << "> Receive thread started. Waiting for messages..." << "\n";

    while (true) {
        memset(buffer, 0, 1024);

           
        int bytesReceived = recvfrom(clientSocket, buffer, 1024, 0, (sockaddr*)&fromAddr, &fromLen);

        if (bytesReceived == SOCKET_ERROR) {
            std::cout << "recvfrom failed: " << WSAGetLastError() << "\n";
            break;
        }
        if (bytesReceived == 0) {
            // close the socket
            break;
        }

        // print the received data
        std::string recvData(buffer, bytesReceived);
        std::cout << "\n> received: " << recvData << "\n> "; 

        if (recvData == "quit") {
            break;
        }
    }
}

int main() {
    // initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed." << "\n";
        return 1;
    }

    // create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed." << "\n";
        WSACleanup();
        return 1;
    }

    // bind the socket 
    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr); 
    clientAddr.sin_port = htons(0);

    if (bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        std::cout << "Bind failed: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    
    sockaddr_in assignedAddr;
    int assignedLen = sizeof(assignedAddr);
    getsockname(clientSocket, (sockaddr*)&assignedAddr, &assignedLen);
    std::cout << "> echo-client activated on 127.0.0.1:" << ntohs(assignedAddr.sin_port) << "\n";

    // create a server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST.c_str(), &serverAddr.sin_addr);

    // create a client thread
    std::thread clientThread(recvHandler, clientSocket);
    clientThread.detach();

    // send messages
    while (true) {
        std::string sendMsg;
        // input the message
        std::getline(std::cin, sendMsg);

        if (sendMsg.empty()) continue;

        int sendResult = sendto(clientSocket, sendMsg.c_str(), sendMsg.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (sendResult == SOCKET_ERROR) {
            std::cout << "sendto failed: " << WSAGetLastError() << "\n";
        }

        if (sendMsg == "quit") {
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    std::cout << "> echo-client is de-activated" << "\n";
    return 0;
}