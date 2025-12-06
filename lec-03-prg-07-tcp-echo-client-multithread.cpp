#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> 
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

const std::string HOST = "127.0.0.1";
const int PORT = 65456;

//{CHAT#1}
void recvHandler(SOCKET clientSocket){
    char buffer[1024];
    while (true){
        memset(buffer, 0, 1024);
        
        int bytes_received = recv(clientSocket, buffer, 1024, 0);

        if(bytes_received == SOCKET_ERROR || bytes_received == 0) {
            std::cout << "[System] Server disconnected.\n";
            break;
        }

        // convert the received data to a string
        std::string recvData(buffer, bytes_received);
        std::cout << "received: " << recvData << "\n";

        if(recvData == "quit"){
            break;
        }
    }
}

int runClient(){
    // initialize a socket
    WSADATA w;
    if(WSAStartup(MAKEWORD(2,2), &w) != 0) return 1;

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    
    inet_pton(AF_INET, HOST.c_str(), &server_addr.sin_addr);
    server_addr.sin_port = htons(PORT);

   
    if (connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cout << "connect failed with error: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    //{CHAT#2}
    std::thread clientThread(recvHandler, clientSocket);
    clientThread.detach();

    while (true) {
        //{start}
        std::string sendMsg;
        std::getline(std::cin, sendMsg);

        
        if(sendMsg.empty()) continue;

        int sendResult = send(clientSocket, sendMsg.c_str(), sendMsg.length(), 0);
        
        if (sendResult == SOCKET_ERROR) {
            std::cout << "send failed\n";
            break;
        }

        if(sendMsg == "quit") {
            break;
        }
        //{end}
    }
    
    // close the socket
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

int main(){
    std::cout << "echo_client is activated" << "\n";
    runClient();
    std::cout << "echo_client is de-activated" << "\n";
    return 0;
}