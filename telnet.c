#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include "telnet.h"

int telnet_connect(char *username, char *password, char *hostname) {
     int status;
     struct addrinfo hints;
     struct addrinfo *servinfo;
     char *buf = calloc(1025, 1);
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
     unsigned char sendbuf[256] = "\xff\xfb\x22"; // will linemode
     send(sock, sendbuf, 3, 0);
     recv(sock, buf, 1024, 0);
     memset(buf, 0, 1024);
     // won't terminal speed, terminal type, x display, new env option
     memcpy(sendbuf, "\xff\xfc\x18\xff\xfc\x20\xff\xfc\x23\xff\xfc\x27", 12);
     send(sock, sendbuf, 12, 0);
     recv(sock, buf, 1024, 0);
     memset(buf, 0, 1024);
     // won't echo, negotiate about window size, remote flow control
     // don't status
     memcpy(sendbuf, "\xff\xfc\x01\xff\xfc\x1f\xff\xfe\x05\xff\xfc\x21", 12);
     send(sock, sendbuf, 12, 0);
     recv(sock, buf, 1024, 0);
     memset(buf, 0, 1024);
     recv(sock, buf, 1024, 0);
     memset(buf, 0, 1024);
     // send username
     int i;
     for (i=0; i < strlen(username); i++) {
          char toSend = username[i];
          memcpy(sendbuf, &toSend, 1);
          send(sock, sendbuf, 1, 0);
          recv(sock, buf, 1, 0);
          memset(buf, 0, 1024);
     }
     // send new line
     memcpy(sendbuf, "\r", 1);
     send(sock, sendbuf, 1, 0);
     // recv new line, then "Password: "
     recv(sock, buf, 2, 0);
     memset(buf, 0, 1024);
     sleep(1);
     recv(sock, buf, 1024, 0);
     memset(buf, 0, 1024);
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
     memset(buf, 0, 1024);
     sleep(1);
     // now check if this password worked
     recv(sock, buf, 1024, 0);
     // close the socket
     close(sock);
     // Check that we don't have another login or password prompt, but
     // instead a last login prompt (standard for success)
     if (strstr(buf, "last login") == NULL &&
          strstr(buf, "ogin:") != NULL && strstr(buf, "incorrect") == NULL) {
               free(buf);
               return 1;
          }
     else {
          free(buf);
          return 0;
     }
}

int telnet_process(char *username, char *password, int single_user,
     int single_password, char *hostname) {
     int numSuccess = 0;
     int result;
     if (single_user == 1 && single_password == 1) {
          result = telnet_connect(username, password, hostname);
          if (result == 1) {
               printf("username: %s password: %s MATCH!\n", username,
               password);
               numSuccess++;
          }
          return numSuccess;
     }
     if (single_user == 1 && single_password == 0) {
          FILE *fp;
          char *line_password;
          size_t len = 0;
          ssize_t read;

          fp = fopen(password, "r");
          if (fp == NULL) {
               perror("open");
               exit(EXIT_FAILURE);
          }
          while ((read = getline(&line_password, &len, fp)) != - 1) {
               line_password[strlen(line_password)-1] = '\0';
               result = telnet_connect(username, line_password, hostname);
               if (result == 1) {
                    printf("username: %s password: %s MATCH!\n", username,
                    line_password);
                    numSuccess++;
               }
          }
          fclose(fp);
          return numSuccess;
     }
     if (single_user == 0 && single_password == 1) {
          FILE *fp;
          char *line_username;
          size_t len = 0;
          ssize_t read;

          fp = fopen(username, "r");
          if (fp == NULL) {
               perror("open");
               exit(EXIT_FAILURE);
          }
          while ((read = getline(&line_username, &len, fp)) != - 1) {
               line_username[strlen(line_username)-1] = '\0';
               result = telnet_connect(line_username, password, hostname);
               if (result == 1) {
                    // remove newline
                    printf("username: %s password: %s MATCH!\n", line_username,
                    password);
                    numSuccess++;
               }
          }
          fclose(fp);
          return numSuccess;
     }
     else {
          FILE *fp_user, *fp_pass;
          char *line_username, *line_password;
          size_t len_user = 0;
          size_t len_pass = 0;
          ssize_t read_user, read_pass;

          fp_user = fopen(username, "r");
          if (fp_user == NULL) {
               perror("open user");
               exit(EXIT_FAILURE);
          }
          while ((read_user = getline(&line_username, &len_user, fp_user))
               != - 1) {
                    // remove newline
                    line_username[strlen(line_username)-1] = '\0';
                    fp_pass = fopen(password, "r");
                    if (fp_pass == NULL) {
                         perror("open pass");
                         exit(EXIT_FAILURE);
                    }
               while ((read_pass = getline(&line_password, &len_pass, fp_pass))
                    != -1) {
                         // remove newline
                         line_password[strlen(line_password)-1] = '\0';
                         result = telnet_connect(line_username, line_password,
                              hostname);
                         if (result == 1) {
                              printf("username: %s password: %s MATCH!\n",
                              line_username, line_password);
                              numSuccess++;
                         }
                    }
                    fclose(fp_pass);
               }
          fclose(fp_user);
          return numSuccess;
     }
}
