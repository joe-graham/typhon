// arghandler.c - Handles command line arguments for Typhon.
// Return values:
// 1 - Incorrect number of arguments
#include <stdio.h>
#include <unistd.h>
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
     arg_package.valid_args = 1;
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
     return arg_package;
}
