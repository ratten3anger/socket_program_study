#ifndef _MULTIPROCESSSTUDY_H
#define _MULTIPROCESSSTUDY_H
#endif

#include <netdb.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 1234
//number of connection
#define BACKLOG 5
#define MAXDATASIZE 1000
