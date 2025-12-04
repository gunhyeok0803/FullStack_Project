#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// {CHAT#1} Create a DB
std::vector<sockaddr_in> group_queue;

bool operator==(const sockaddr_in& a, const sockaddr_in& b) {
    return (a.sin_addr.s_addr == b.sin_addr.s_addr) && (a.sin_port == b.sin_port);
}

std::string strip(const std::string& str) {
    const char* whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return ""; 
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in server_addr = { AF_INET, htons(65456) };
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind Failed" << "\n";    
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    
    std::cout << "> echo-server is activated" << "\n";

    char buffer[1024];
    sockaddr_in client_addr;
    int addr_len = sizeof(client_addr);

    // server loop
    while (true) {
        // {start}
        memset(buffer, 0, 1024);
        int bytes = recvfrom(server_socket, buffer, 1024, 0, (sockaddr*)&client_addr, &addr_len);
        if (bytes <= 0) continue;

        std::string raw_data(buffer, bytes);
        std::string RecvCmd = strip(raw_data); 
        
        // {CHAT#2} Command line protocol
        if ((!RecvCmd.empty() && RecvCmd[0] == '#') || RecvCmd == "quit") {
            
            if (RecvCmd == "#REG") {
                // Python: print -> append
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
                std::cout << "> client registered " << ip << ":" << ntohs(client_addr.sin_port) << "\n";
                
                
                bool exists = false;
                for(const auto& c : group_queue) if(c == client_addr) exists = true;
                if(!exists) group_queue.push_back(client_addr);
            }
            else if (RecvCmd == "#DEREG" || RecvCmd == "quit") {
                bool contains = false;
                for(const auto& c : group_queue) if(c == client_addr) contains = true;

                if (contains) {
                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
                    std::cout << "> client de-registered " << ip << ":" << ntohs(client_addr.sin_port) << "\n";
                    
                    auto it = std::remove(group_queue.begin(), group_queue.end(), client_addr);
                    group_queue.erase(it, group_queue.end());
                }
            }
        }
        else {
            // {CHAT#3} Prohibit an un-registered client message
            
            // check if registered
            bool is_registered = false;
            for(const auto& c : group_queue) if(c == client_addr) is_registered = true;

            
            if (group_queue.empty()) {
                std::cout << "> no clients to echo" << "\n";
            }
            
            else if (!is_registered) {
                std::cout << "> ignores a message from un-registered client" << "\n";
            }
            else {
                // {CHAT#4}  Forward a client message to whole clients (currently a broadcast)
                std::cout << "> received ( " << RecvCmd << " ) and echoed to " << group_queue.size() << " clients" << "\n";
                
                for (const auto& clientConn : group_queue) {
                    sendto(server_socket, raw_data.c_str(), bytes, 0, (sockaddr*)&clientConn, sizeof(clientConn));
                }
            }
        }
        // {end}
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}