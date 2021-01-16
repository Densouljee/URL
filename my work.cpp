#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")


void error(const char* msg) { perror(msg); exit(0); }

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);

    int portno = 80;
    char host[1024];
    char url[1024];

    printf("Enter host:\n");
    scanf("%s", host);
    printf("Enter URL:\n");
    scanf("%s", url);

    const char* message_fmt;
    message_fmt = "GET %s HTTP/1.0\r\n\r\n";

    struct hostent* server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024], response[4096];

    sprintf(message, message_fmt, url);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    server = gethostbyname(host);
    if (server == NULL) error("ERROR, no such host");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    total = strlen(message);
    sent = 0;
    do {
        bytes = send(sockfd, message + sent, total - sent, 0);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent += bytes;
    } while (sent < total);

    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
    do {
        bytes = recv(sockfd, response + received, total - received, 0);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received += bytes;
    } while (received < total);

    closesocket(sockfd);

    char* html = strstr(response, "\r\n\r\n") + 4;
    printf("File size %d bytes\n", strlen(html) + 2);

    FILE* f = fopen("response.txt", "w");
    fprintf(f, "%s", html);
    fclose(f);

    WSACleanup();

    return 0;
}