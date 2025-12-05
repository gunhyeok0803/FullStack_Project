#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

void main_server() {
    // Initialize Winsock
    WSADATA w;
    if (WSAStartup(MAKEWORD(2, 2), &w) != 0){
        std::cout << "WSAStartup failed." << "\n";
        return;
    }

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET){
        std::cout << "Socket creation failed." << "\n";
        WSACleanup();
        return;
    }

    // binding socket
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (InetPtonA(AF_INET, HOST, &serverAddr.sin_addr) !=1){
        std::cerr << "Invalid IP address." << "\n";
    }
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        std::cout << "Bind failed." << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Listening
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR){
        std::cout << "Listen failed." << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Accepting
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET){
        std::cout << "Accept failed." << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // connected
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
    std::cout << "client connected by IP address" << clientIP << "with port" << ntohs(clientAddr.sin_port) << "\n";

    //ehco loop
    char buffer[1024];
    while (true){
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        // error handing
        if (bytesReceived <= 0){
            std::cout << "recv failed." << "\n";
            break;
        }

        // transform buffer to string
        std::string recvMsg(buffer, bytesReceived);
        std::cout << "received: " << recvMsg << "\n";

        //send
        send(clientSocket, recvMsg.c_str(), (int)recvMsg.size(), 0);
    }

    // close socket
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

int main(){
    std::cout << "ehco server is activated" << "\n";
    main_server();
    std::cout << "ehco server is deactivated" << "\n";
    return 0;
}