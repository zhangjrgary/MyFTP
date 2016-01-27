#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/stat.h>

#define BUFSIZE 1024
#define PATH_LEN 108


#define SERVER_IP "192.168.1.100"
#define SERVER_PORT 8000


typedef struct sockaddr SA;
typedef struct sockaddr_in SA_in;


