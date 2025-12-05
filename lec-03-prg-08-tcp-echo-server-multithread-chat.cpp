#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <winsock2.h> 
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Winsock lib


// {CHAT#1} Create a DB to register all client's socket information
std::vector<SOCKET> group_queue;
std::mutex queue_mutex; // Mutex

std::atomic<int> active_thread_count(0);

class ThreadedTCPRequestHandler {
public:
    void handle(SOCKET clientSocket, struct sockaddr_in clientAddr){
        active_thread_count ++;

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        std::cout << "client connected by IP address" << clientIP << "with Port number " << clientPort << "\n";
        // {CHAT#3} Register a new client connection information into a client DB   
        {    std::lock_guard<std::mutex> lock(queue_mutex); 
             group_queue.push_back(clientSocket);
        }
    
        char buffer[1024];
        while(true){
            //{start}
            memset(buffer, 0, 1024);
            int bytesReceived = recv(clientSocket, buffer, 1024, 0);

            std::string recvStr = std::string(buffer);

            if(bytesReceived == 0 || recvStr == "quit"){
                // {CHAT#4} Deregister a disconnected client from a client DB
                std::lock_guard<std::mutex> lock(queue_mutex);
                std::cout << "> received ( " << recvStr << " ) and echoed to " << group_queue.size() << " clients" << "\n";
                
                for(SOCKET conn : group_queue){
                    send(conn, buffer, bytesReceived, 0);
                }
            }
        }
        //{end}
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

        // create a socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, host, &serverAddr.sin_addr);

        // Bind
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }
        
        // Listen
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
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
            // Start a thread with the server
            ThreadedTCPRequestHandler handler;
            std::thread clientThread(&ThreadedTCPRequestHandler::handle, handler, clientSocket, clientAddr);
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

    std::cout << "ehco-server is activated" << "\n";

    ThreadedTCPServer server(HOST, PORT);

    // Start a thread with the server
    std::thread server_thread(&ThreadedTCPServer::server_forever, &server);
    server_thread.detach();

    std::cout << "> server loop running in thread (main thread)" << "\n";
    while (true) {
        std::string msg;
    
        std::getline(std::cin, msg); // input('> ')

        if (msg == "quit") {
            if (active_thread_count == 0) {
                std::cout << "> stop procedure started" << "\n";
                break;
            }
            else {
                std::cout << "> active threads are remained : " << active_thread_count << " threads" << "\n";
            }
        }
    }

    std::cout << "> echo-server is de-activated" << "\n";
    server.shutdown();

    return 0;
}