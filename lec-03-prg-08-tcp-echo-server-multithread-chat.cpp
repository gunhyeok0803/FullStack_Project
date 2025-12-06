#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm> 
#include <winsock2.h> 
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") 


std::vector<SOCKET> group_queue;
std::mutex queue_mutex; 
std::atomic<int> active_thread_count(0);

class ThreadedTCPRequestHandler {
public:
    void handle(SOCKET clientSocket, struct sockaddr_in clientAddr){
        active_thread_count++;

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        std::cout << "[System] Client connected: " << clientIP << ":" << clientPort << "\n";
        
        {    
            std::lock_guard<std::mutex> lock(queue_mutex); 
            group_queue.push_back(clientSocket);
        }
    
        char buffer[1024];
        while(true){
            memset(buffer, 0, 1024);
            
            // receive data
            int bytesReceived = recv(clientSocket, buffer, 1024, 0);

            
            if (bytesReceived > 0) {
                 // std::cout << "[DEBUG] Raw bytes received: " << bytesReceived << "\n";
            }

            
            if(bytesReceived <= 0) {
                break; 
            }

            std::string recvStr(buffer, bytesReceived);
            
            
            if (!recvStr.empty() && recvStr.back() == '\n') recvStr.pop_back();
            if (!recvStr.empty() && recvStr.back() == '\r') recvStr.pop_back();

          
            if(recvStr == "quit"){
                break; 
            }

            
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                std::cout << "> received ( " << recvStr << " ) and echoed to " << group_queue.size() << " clients\n";
                
                for(SOCKET conn : group_queue){
                    // send to all clients
                    send(conn, buffer, bytesReceived, 0);
                }
            }
        }

        // close connection
        std::cout << "[System] Client disconnected: " << clientIP << ":" << clientPort << "\n";
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            auto it = std::remove(group_queue.begin(), group_queue.end(), clientSocket);
            if (it != group_queue.end()) {
                group_queue.erase(it, group_queue.end());
            }
        }

        closesocket(clientSocket);
        active_thread_count--;
    }
};

class ThreadedTCPServer{
    SOCKET serverSocket;
    bool isRunning;

    public:
    ThreadedTCPServer(const char* host, int port) : isRunning(false) {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, host, &serverAddr.sin_addr);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            return;
        }
        
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
            return;
        }
    }

    void server_forever(){
        isRunning = true;
        while(isRunning){
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            SOCKET clientSocket = accept(serverSocket,(struct sockaddr*)&clientAddr, &clientAddrSize);

            if(clientSocket == INVALID_SOCKET){
                if(!isRunning) break;
                continue;
            }
            
            std::thread clientThread(&ThreadedTCPRequestHandler::handle, ThreadedTCPRequestHandler(), clientSocket, clientAddr);
            clientThread.detach();
        }
    }

    void shutdown() {
        isRunning = false;
        closesocket(serverSocket);
        WSACleanup();
    }
};

int main(){
    const char* HOST = "127.0.0.1";
    int PORT = 65456;

    std::cout << "echo-server is activated" << "\n";

    ThreadedTCPServer server(HOST, PORT);

    std::thread server_thread(&ThreadedTCPServer::server_forever, &server);
    server_thread.detach();

    std::cout << "> server loop running in thread (main thread)" << "\n";
    while (true) {
        std::string msg;
        std::getline(std::cin, msg); 

        if (msg == "quit") {
            if (active_thread_count == 0) {
                std::cout << "> stop procedure started" << "\n";
                break;
            }
            else {
                std::cout << "> active threads remained : " << active_thread_count << " threads" << "\n";
            }
        }
    }

    std::cout << "> echo-server is de-activated" << "\n";
    server.shutdown();

    return 0;
}