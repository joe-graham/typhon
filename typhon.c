#include <stdio.h>
#include "arghandler.h"

int main(int argc, char **argv) {
     struct args arg_package;
     arg_package = arg_handler(argc, argv);
     if (arg_package.valid_args == 1) {
          printf("Username: %s\n", arg_package.username);
          printf("Password: %s\n", arg_package.password);
     }
     return 0;
}
