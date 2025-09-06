#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    // Creating socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0) // IPv4 TCP socket
    // Binding socket to the IP and port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addrp.s_addr = inet_addr("127.0.0.1"); // Localhost bind
    address.sin_port = htons(8080); // Port 8080
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    // Listen for incomming connections
    listen(server_fd, 3); // Max of 3 pending connections

    printf("Server is running and listening on port 8080...\n")

    while (1) {
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen); // Three way handshake with accept()

        if (new_socket < 0){
            perror("Failed to accept connection");
            continue;
        }
        printf("Connection accepted.\n");
    }
    
    // Handling data transfer
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024); //Read data sent by the client
    printf("Client message %s\n", buffer) // Print the client's message
    
    // Respond to the client
    char *response = "Hello from the server.";
    write(new_socket, response, strlen(response)); // Sending data to the client

    close(new_scocket); // Close the client connection


}