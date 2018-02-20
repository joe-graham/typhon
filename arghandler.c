// arghandler.c - Handles command line arguments for Typhon.
// Return values:
// 1 - Incorrect number of arguments
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "arghandler.h"

void print_usage() {
     fprintf(stderr, "typhon [options] service://ip\n");
     fprintf(stderr, "Options:\n");
     fprintf(stderr, "-u - Single username to use when connecting to the service.\n");
     fprintf(stderr, "-U - File that contains each username to try, one "\
          "username per line.\n");
     fprintf(stderr, "-p - Single password to use when connecting to the service.\n");
     fprintf(stderr, "-P - File that contains each password to try, one "\
          "password per line.\n");
     fprintf(stderr, "Supported protocols are FTP, SSH, and telnet.\n");
     return;
}

struct args arg_handler(int argc, char **argv) {
     struct args arg_package;
     if (argc != 6) {
          arg_package.valid_args = 0;
          print_usage();
          return arg_package;
     }
     // Make sure the service/hostname is actually given before we start
     // populating the arg_package.
     char *service_and_hostname = argv[5];
     char *colon = strchr(service_and_hostname, ':');
     if (colon == NULL) {
          arg_package.valid_args = 0;
          print_usage();
          return arg_package;
     }
     // Make sure there are two slashes after the colon.
     if (colon[1] != 47 || colon[2] != 47) {
          arg_package.valid_args = 0;
          print_usage();
          return arg_package;
     }
     // We have valid arguments, now loop over argv and set arg_package values
     // based on what we find.
     const char *options = "-u: -U: -p: -P:";
     int getopt_return;
     while ((getopt_return = getopt(argc, argv, options)) != -1) {
          char option = (char)getopt_return;
          switch (option) {
               case 117: // u
                    arg_package.single_user = 1;
                    arg_package.username = optarg;
                    break;
               case 85: // U
                    arg_package.single_user = 0;
                    arg_package.username = optarg;
                    break;
               case 112: // p
                    arg_package.single_password = 1;
                    arg_package.password = optarg;
                    break;
               case 80: // P
                    arg_package.single_password = 0;
                    arg_package.password = optarg;
                    break;
          }
     }
     int colon_index = (int)colon - (int)service_and_hostname;
     char service[7];
     memcpy(service, service_and_hostname, colon_index);
     service[7] = '\0'; // Make sure last value is a null byte.
     char *hostname = colon += 3; // Hostname is past the colon and two slashes.
     arg_package.valid_args = 1;
     return arg_package;
}
