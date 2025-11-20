#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

void main_client() {
    // Initialize Winsock
    WSADATA w;
    if (WSAStartup(MAKEWORD(2, 2), &w) != 0) {
        std::cout << "WSAStartup failed." << "\n";
        return;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed." << "\n";
        WSACleanup();
        return;
    }

    //Connecting
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (InetPtonA(AF_INET, HOST, &serverAddr.sin_addr) != 1){
        std::cout << "Invalid IP address" << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    if(connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        std::cout << "> connect() failed and program terminated" << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // echo loop
    char buffer[1024];
    std::string sendMsg;

    while(true){
       // input
       std::getline(std::cin, sendMsg);
    
       // send
       int sendResult = send(clientSocket, sendMsg.c_str(), (int)sendMsg.length(), 0);
       if (sendResult == SOCKET_ERROR) {
         std::cout << "send failed." << "\n";
         break;
        }

       //receive
       int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

       //error handing
       if(bytesReceived == 0){
         break;
        }

       //buffer to string
       std::string recvMsg(buffer, bytesReceived);
       std::cout << "received: " << recvMsg << "\n";
       if(sendMsg == "quit"){
         break;
        }
    }

    //close socket
    closesocket(clientSocket);
    WSACleanup();
}


int main() {
    std::cout << "> echo-client is activated" << "\n";
    main_client();
    std::cout << "> echo-client is de-activated" << "\n";
    return 0;
}