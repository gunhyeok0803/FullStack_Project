#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <sstream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

std::atomic<int> active_thread_count(1);

class ThreadedTCPRequestHandler {
public:
    SOCKET request;
    sockaddr_in client_address;

    ThreadedTCPRequestHandler(SOCKET sock, sockaddr_in addr) 
        : request(sock), client_address(addr) {}

    void handle() {
        // Show a client connection information
        std::cout << "client connected by IP address " << inet_ntoa(client_address.sin_addr) 
                  << " with Port number " << ntohs(client_address.sin_port) <<"\n";

        char RecvDataBuffer[1024];

        while (true) {
            // [=start=]
            int bytes = recv(request, RecvDataBuffer, 1024, 0);
            
            if (bytes <= 0) break;

            std::string RecvData(RecvDataBuffer);
            
            std::stringstream ss;
            ss << "Thread-" << std::this_thread::get_id();
            std::string cur_thread_name = ss.str();

            std::cout << "echoed: " << RecvData << " by " << cur_thread_name << "\n";
            
            send(request, RecvDataBuffer, bytes, 0);
            
            if (RecvData == "quit") {
                break;
            }
            // [==end==]
        }
        closesocket(request);
    }
};


class ThreadedTCPServer {
public:
    SOCKET server_socket;
    sockaddr_in server_address;
    bool is_running;

    ThreadedTCPServer(std::pair<std::string, int> addr, void* handler_cls) {
        // initialize a socket 
        WSAData w;
        WSAStartup(MAKEWORD(2, 2), &w);

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr(addr.first == "localhost" ? "127.0.0.1" : addr.first.c_str());
        server_address.sin_port = htons(addr.second);

        if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
            closesocket(server_socket);
            WSACleanup();
            std::cout << "bind failed" << "\n";
        }
        if (listen(server_socket, 5) == SOCKET_ERROR) {
            closesocket(server_socket);
            WSACleanup();
            std::cout << "listen failed" << "\n";
        }
    }
    

    void serve_forever() {
        while (is_running) {
            sockaddr_in client_addr;
            int client_len = sizeof(client_addr);
            SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

            if (client_socket == INVALID_SOCKET) continue;

            // ThreadingMixIn
            active_thread_count++;
            std::thread([client_socket, client_addr](){
                ThreadedTCPRequestHandler (client_socket, client_addr).handle();
                active_thread_count--;
            }).detach();
        }
    }

    void shutdown() {
        is_running = false;
        closesocket(server_socket);
        WSACleanup();
    }
};

int main() {
    std::string HOST = "localhost";
    int PORT = 65456;
    std::cout << "echo-server is activated" << "\n";

    ThreadedTCPServer server({HOST, PORT}, nullptr);

    // ThreadingMixIn
    active_thread_count++;
    std::thread server_thread([&server]() {
        server.serve_forever();
        active_thread_count--; 
    });

    // daemon
    server_thread.detach(); 
    std::cout << "server loop running in thread: " << "MainThread" <<"\n";

    
    int baseThreadNumber = active_thread_count.load(); // account 호출

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);

        if (msg == "quit") {
            if (baseThreadNumber == active_thread_count.load()) {
                std::cout << "stop procedure started" << "\n";
                break;
            }
            else {
                std::cout << "active threads are remained : " << (active_thread_count.load() - baseThreadNumber) << " threads" << "\n";
            }
        }
    }

    std::cout << "echo-server is de-activated" <<"\n";
    server.shutdown();
    
    return 0;
}