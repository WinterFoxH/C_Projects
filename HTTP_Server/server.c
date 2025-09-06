#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 2048

int main() {
    WSADATA wsaData;
    int wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaerr) {
        printf("WSAStartup failed\n");
        return 1;  
    }
    
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 TCP socket
    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in address;
    // Binding socket to the IP and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost bind
    address.sin_port = htons(PORT); // Port 8080
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) == SOCKET_ERROR) { // Max of 3 pending connections
        printf("Listen failed.\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    
    printf("HTTP Server is running on http://127.0.0.1:%d\n", PORT);
    printf("Press Ctrl+C to stop the server.\n");

    while (1) {
        int addrlen = sizeof(address);
        SOCKET new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);

        if (new_socket == INVALID_SOCKET) {
            printf("Failed to accept connection\n");
            continue;
        }
        printf("Connection accepted from client.\n");
    
        // Handle HTTP request
        char request[BUFFER_SIZE] = {0};
        int bytes_received = recv(new_socket, request, sizeof(request) - 1, 0);
        
        if (bytes_received > 0) {
            request[bytes_received] = '\0'; // Ensure null termination
            printf("Client request:\n%s\n", request);

            // Handle favicon.ico request separately
            if (strncmp(request, "GET /favicon.ico", 16) == 0) {
                char *notfound = "HTTP/1.1 404 Not Found\r\n"
                                "Content-Length: 0\r\n"
                                "Connection: close\r\n"
                                "\r\n";
                send(new_socket, notfound, strlen(notfound), 0);
                printf("Served 404 for favicon.ico\n");
            } 
            else {
                // Serve index.html as default
                FILE *f = fopen("index.html", "rb");
                if (!f) {
                    char *notfound = "HTTP/1.1 404 Not Found\r\n"
                                    "Content-Type: text/html\r\n"
                                    "Content-Length: 47\r\n"
                                    "Connection: close\r\n"
                                    "\r\n"
                                    "<html><body><h1>404 Not Found</h1></body></html>";
                    send(new_socket, notfound, strlen(notfound), 0);
                    printf("Served 404 - index.html not found\n");
                } 
                else {
                    // Get file size
                    fseek(f, 0, SEEK_END);
                    long filesize = ftell(f);
                    fseek(f, 0, SEEK_SET);

                    // Read file content
                    char *body = malloc(filesize + 1);
                    if (body) {
                        fread(body, 1, filesize, f);
                        body[filesize] = '\0';
                        fclose(f);

                        // Send HTTP response
                        char header[512];
                        snprintf(header, sizeof(header),
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: %ld\r\n"
                            "Connection: close\r\n"
                            "\r\n",
                            filesize);

                        send(new_socket, header, strlen(header), 0);
                        send(new_socket, body, filesize, 0);
                        printf("Served index.html (%ld bytes)\n", filesize);

                        free(body);
                    } else {
                        printf("Memory allocation failed\n");
                        fclose(f);
                    }
                }
            }
        }
        
        closesocket(new_socket);
        printf("Connection closed.\n\n");
    }
    
    closesocket(server_fd);
    WSACleanup();
    return 0;
}