#ifndef IMAN_H
#define IMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 4096
#define HOST "man.he.net"
#define PORT 80

void iman(char *man_page);

#endif 