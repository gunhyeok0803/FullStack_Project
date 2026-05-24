#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

std::string HOST = "127.0.0.1";
int PORT = 65456;

//{CHAT#1}
void revHandler(SOCKET clientSocket){
    char buffer[1024];
    while (true){
        int bytes_received = recv(clientSocket, buffer, 1024, 0);

        if(bytes_received == 0) break;

        std::cout << "received: " << buffer << "\n";

        if(std::string(buffer) == "quit"){
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
    server_addr.sin_addr.s_addr = inet_addr(HOST.c_str());
    server_addr.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cout << "connect falid" << "\n";

        std::cout << "connect failed by exceptin: " << WSAGetLastError() << "\n";

        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    //{CHAT#2}
    std::thread clientTread(revHandler, clientSocket);
    clientTread.detach();

    while (true) {
        //{start}
        std::string sendMsg;
        std::getline(std::cin, sendMsg);

        send(clientSocket, sendMsg.c_str(), sendMsg.length(), 0);

        if(sendMsg == "quit") {
            break;
        }
        //{end}
    }
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