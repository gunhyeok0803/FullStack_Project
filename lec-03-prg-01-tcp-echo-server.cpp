#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

int main() {
	// winsock initialization
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << "\n";
		return 1;
	}

	// server socket creation
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed" << "\n";
		WSACleanup();
		return 1;
	}

	// server address structure
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr);

	serverAddr.sin_port = htons(65456); 

	// binding socket
	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed" << "\n";
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	// listening for incoming connections
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed" << "\n";
	    closesocket(serverSocket);
	    WSACleanup();
	    return 1;
	}

	std::cout << "Server is listening on port 65456..." << "\n";

	// accepting a client connection
	while (true) {
		sockaddr_in clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept failed" << "\n";
			continue;
		}

		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
		std::cout << "> client connected by IP address " << clientIP << "\n";

		
		// echo loop
		char buffer[1024];
		int bytesReceived;
		while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
			send(clientSocket, buffer, bytesReceived, 0); // echo back to client
		}
		std::cout << "Client disconnected: " << clientIP << "\n";
		closesocket(clientSocket);

		closesocket(serverSocket);
	}
	WSACleanup();
}






