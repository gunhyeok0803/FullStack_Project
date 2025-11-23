#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024

class MYTCPSocketHandler {
private:
    SOCKET request;             
    sockaddr_in client_address; 

public:
    MYTCPSocketHandler(SOCKET sock, sockaddr_in addr) : request(sock), client_address(addr) {}

    // handler
    void handle() {
        std::cout << "> client connected by IP address " << inet_ntoa(client_address.sin_addr) 
                  << " with Port number " << ntohs(client_address.sin_port) <<"\n";

        char buffer[BUFFER_SIZE];
        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(request, buffer, BUFFER_SIZE, 0);

            
            if (bytes_received == 0) break;

            std::cout << "> echoed: " << buffer << "\n";
            send(request, buffer, bytes_received, 0);

            if (std::string(buffer) == "quit") {
                break;
            }
           
        }
    }
};

int main() {
    // Create a socket
    const int PORT = 65456;
    
    WSADATA w;
    WSAStartup(MAKEWORD(2, 2), &w);

    SOCKET socket_server = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (bind(socket_server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "> bind() failed" << "\n";
        closesocket(socket_server);
        WSACleanup();
        return 1;
    }
    if (listen(socket_server, 5) == SOCKET_ERROR) {
        std::cerr << "> listen() failed" << "\n";
        closesocket(socket_server);
        WSACleanup();
        return 1;
    }

    std::cout << "> echo-server is activated" << "\n";

    // while loop
    while (true) {
        sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        
        // Accepting
        SOCKET client_socket = accept(socket_server, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) continue;

        // handler
        MYTCPSocketHandler handler(client_socket, client_addr);
        handler.handle();

      
        closesocket(client_socket);
    }

    closesocket(socket_server);
    WSACleanup();
    std::cout << "> echo-server is de-activated" << "\n";

    return 0;
}