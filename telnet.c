#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include "telnet.h"

int telnet_connect(char *username, char *password, char *hostname) {
     int status;
     struct addrinfo hints;
     struct addrinfo *servinfo;
     char buf[1025];
     int sock;
     int recvbytes;
     memset(&hints, 0, sizeof(hints));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_STREAM;
     status = getaddrinfo(hostname, "23", &hints, &servinfo);
     char output[15];
     if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype,
          servinfo->ai_protocol)) == -1) {
               close(sock);
               perror("socket");
               return -1;
          }
     int flag = 1;
     // Telnet sends one character long payloads for passwords, which
     // needs the Nagle algorithm to be disabled for it to work
     int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag,
          sizeof(int));
     if (result < 0) {
          perror("setsockopt");
     }
     if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
          close(sock);
          perror("connect");
          }
     if ((recvbytes = recv(sock, buf, 1024, 0)) == -1) {
          perror("recv");
          return -1;
     }
     buf[1025] = '\0';
     unsigned char sendbuf[256] = "\xff\xfb\x22"; // will linemode
     send(sock, sendbuf, 3, 0);
     recv(sock, buf, 1024, 0);
     // won't terminal speed, terminal type, x display, new env option
     memcpy(sendbuf, "\xff\xfc\x18\xff\xfc\x20\xff\xfc\x23\xff\xfc\x27", 12);
     send(sock, sendbuf, 12, 0);
     recv(sock, buf, 1024, 0);
     // won't echo, negotiate about window size, remote flow control
     // don't status
     memcpy(sendbuf, "\xff\xfc\x01\xff\xfc\x1f\xff\xfe\x05\xff\xfc\x21", 12);
     send(sock, sendbuf, 12, 0);
     recv(sock, buf, 1024, 0);
     recv(sock, buf, 1024, 0);
     // send username
     int i;
     for (i=0; i < strlen(username); i++) {
          char toSend = username[i];
          memcpy(sendbuf, &toSend, 1);
          send(sock, sendbuf, 1, 0);
          recv(sock, buf, 1, 0);
     }
     // send new line
     memcpy(sendbuf, "\r", 1);
     send(sock, sendbuf, 1, 0);
     // recv new line, then "Password: "
     recv(sock, buf, 2, 0);
     recv(sock, buf, 1024, 0);
     // send password
     for (i=0; i < strlen(password); i++) {
          char toSend = password[i];
          memcpy(sendbuf, &toSend, 1);
          send(sock, sendbuf, 1, 0);
     }
     // send new line
     memcpy(sendbuf, "\r", 1);
     send(sock, sendbuf, 1, 0);
     recv(sock, buf, 2, 0);
     // now check if this password worked
     recv(sock, buf, 1024, 0);
     // close the socket
     close(sock);
     // Check that we don't have another login or password prompt, but
     // instead a last login prompt (standard for success)
     if (strstr(buf, "last login") == NULL &&
          strstr(buf, "ogin:") != NULL && strstr(buf, "sername:") == NULL) {
               return 1;
          }
     else { return 0; }
}

int telnet_process(char *username, char *password, int single_user,
     int single_password, char *hostname) {
     int numSuccess = 0;
     if (single_user == 1 && single_password == 1) {
          int result = telnet_connect(username, password, hostname);
          if (result == 1) {
               printf("username: %s password: %s MATCH!\n", username,
               password);
               numSuccess++;
          }
          return numSuccess;
     }
}