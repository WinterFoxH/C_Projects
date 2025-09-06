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
    char request[2048] = {0};
    recv(new_socket, request,  sizeof(request),0); //Read data sent by the client
    printf("Client message %s\n", request); // Print the client's message

// Handle favicon.ico request separately
if (strncmp(request, "GET /favicon.ico", 16) == 0) {
    char *notfound = "HTTP/1.1 404 Not Found\r\n"
                     "Content-Length: 0\r\n"
                     "Connection: close\r\n"
                     "\r\n";
    send(new_socket, notfound, strlen(notfound), 0);
} else {
    // Serve index.html as default
    FILE *f = fopen("index.html", "rb");
    if (!f) {
        char *notfound = "HTTP/1.1 404 Not Found\r\n"
                         "Content-Type: text/plain\r\n"
                         "Content-Length: 13\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "404 Not Found";
        send(new_socket, notfound, strlen(notfound), 0);
    } else {
        fseek(f, 0, SEEK_END);
        long filesize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *body = malloc(filesize);
        fread(body, 1, filesize, f);
        fclose(f);

        char header[256];
        sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n"
            "\r\n",
            filesize);

        send(new_socket, header, strlen(header), 0);
        send(new_socket, body, filesize, 0);

        free(body);
    }
}
        closesocket(new_socket);
    }
    closesocket(server_fd);
    WSACleanup();
    return 0;
}