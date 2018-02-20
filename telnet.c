#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "telnet.h"

void telnet_connect(char *username, char *password, char *hostname) {
     int status;
     struct addrinfo hints;
     struct addrinfo *servinfo;
     memset(&hints, 0, sizeof(hints));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_STREAM;
     status = getaddrinfo(hostname, "23", &hints, &servinfo);
}

void telnet_process(char *username, char *password, int single_user, \
     int single_password, char *hostname) {
     if (single_user == 1 && single_password == 1) {
          telnet_connect(username, password, hostname);
     }
}
