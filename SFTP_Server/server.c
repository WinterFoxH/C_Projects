#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>

int main() {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0) // IPv4 TCP socket

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addrp.s_addr = inet_addr("127.0.0.1"); // Localhost bind
    address.sin_port = htons(8080); // Port 8080

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    
    listen(server_fd, 3); // Max of 3 pending connections

    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen); // Three way handshake with accept()

    
}