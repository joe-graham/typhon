#include <stdio.h>
#include <string.h>
#include "arghandler.h"
#include "telnet.h"

int main(int argc, char **argv) {
     struct args arg_package;
     int numSuccess;
     arg_package = arg_handler(argc, argv);
     if (arg_package.valid_args == 0) {
          return 1;
     }
     char *username = arg_package.username;
     char *password = arg_package.password;
     if ((strncmp(arg_package.service, "telnet", 6)) == 0) {
          numSuccess = telnet_process(username, password,
               arg_package.single_user, arg_package.single_password,
               arg_package.hostname);
     }
     printf("Number of matches: %d\n", numSuccess);
     return 0;
}
