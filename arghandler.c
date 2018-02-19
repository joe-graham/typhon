// arghandler.c - Handles command line arguments for Typhon.
// Return values:
// 1 - Incorrect number of arguments
#ifndef _STDIO_
#define _STDIO_
#include <stdio.h>
#endif
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

int arg_handler(int argc, char **argv) {
     if (argc != 3) {
          print_usage();
          return 1;
     }
}
