#include <winsock2.h>
#include <stdio.h>

int main() {

    WSADATA wsaData;
    int wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (wsaerr){
          printf("WSAStartup failed\n");
            return 1;  
        }
    
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 TCP socket
    if (server_fd == INVALID_SOCKET){
        printf("Socket creating failed.\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in address;
    // Binding socket to the IP and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost bind
    address.sin_port = htons(8080); // Port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR){
        printf("Bind failed. \n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen for incomming connections
    listen(server_fd, 3); // Max of 3 pending connections
    printf("Server is running and listening on port 8080...\n");

    while (1) {
        int addrlen = sizeof(address);
        SOCKET new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen); // Three way handshake with accept()

        if (new_socket == INVALID_SOCKET){
            perror("Failed to accept connection");
            continue;
        }
        printf("Connection accepted.\n");
    
    
    // Handling data transfer
    char buffer[1024] = {0};
    recv(new_socket, buffer,  sizeof(buffer),0); //Read data sent by the client
    printf("Client message %s\n", buffer); // Print the client's message
    
    // Respond to the client
    char *response = "Hello from the server.";
    send(new_socket, response, strlen(response), 0); // Sending data to the client

    // Closing the client connection (Four-way handshake)
    closesocket(new_socket); // Close the client connection
    printf("Connection closed.\n");


    }
    // Close the listening socket (Will never be reached in current loop)
    closesocket(server_fd);
    WSACleanup();
    return 0;
}