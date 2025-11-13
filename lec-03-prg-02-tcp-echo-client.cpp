#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")


int main() {
	const char* HOST = "127.0.0.1";
	const unsigned short PORT = 65456;

    std::cout << "> echo-client is activated" << "\n";

	// Initialize Winsock
	WSADATA w;
	if (WSAStartup(MAKEWORD(2, 2), &w) != 0) {
		std::cerr << "WSAStartup failed." << "\n";
		return 1;
	}

	// Create a socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed." << "\n";
		WSACleanup();
		return 1;
	}

    // Connect to server
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (InetPtonA(AF_INET, HOST, &serverAddr.sin_addr) != 1) {
        closesocket(clientSocket); 
        WSACleanup();
        return 1;
    }
    
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(clientSocket); 
        WSACleanup(); 
        return 1;
    }
   
    // Iteractive send and receive
    std::string sendMsg;
    char recvData[1024];
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, sendMsg);

        // clientSocket.sendall(bytes(sendMsg, 'utf-8'))
        ::send(clientSocket, sendMsg.c_str(), (int)sendMsg.size(), 0);

        // recvData = clientSocket.recv(1024)
        int n = ::recv(clientSocket, recvData, (int)sizeof(recvData) - 1, 0);
        if (n > 0) {
            recvData[n] = '\0';
            std::cout << "> received: " << recvData << "\n";
        }

        if (sendMsg == "quit") break;
    }

    closesocket(clientSocket);
    WSACleanup();

    std::cout << "> echo-client is de-activated" << "\n";
}
	
