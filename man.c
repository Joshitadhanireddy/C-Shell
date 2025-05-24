#include "man.h"

void iman(char *man_page) 
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("Error opening socket");
        return;
    }
    server = gethostbyname(HOST);
    if (server == NULL) 
    {
        fprintf(stderr, "Error: no such host\n");
        close(sockfd);
        return;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Error connecting to server");
        close(sockfd);
        return;
    }

    snprintf(request, sizeof(request),
             "GET /man1/%s HTTP/1.0\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             man_page, HOST);

    if (send(sockfd, request, strlen(request), 0) < 0) 
    {
        perror("Error sending request");
        close(sockfd);
        return;
    }
    while ((bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) 
    {
        buffer[bytes_received] = '\0';
        int ttag = 0;
        char *t1 = buffer;
        int j = 0;
        for(int i = 0; t1[i] != '\0'; i++)
        {
            if (t1[i] == '<') 
            {
                ttag = 1;
            } 
            else if (t1[i] == '>') 
            {
                ttag = 0;
            } 
            else if(!ttag)
            {
                buffer[j++]  = t1[i];
            }
        }
        buffer[j] = '\0';
        int offset = 0;
        char *woutheader = buffer;
        for (char *wheader = buffer; (*wheader != '\0') && (offset < 6); *wheader++) 
        {
                if (*wheader == '\n') 
                {
                    offset++;
                }
                if (offset == 6) 
                {
                    *woutheader++ = *wheader;
                    break;
                }
        }
        printf("%s", woutheader);
         
    }

    if (bytes_received < 0) 
    {
        perror("Error receiving response");
    }

    close(sockfd);
}