// arghandler.h - header for arghandler library.
#ifndef _ARGHANDLER_H_
#define _ARGHANDLER_H_

extern struct args arg_handler(int argc, char **argv);
typedef struct args {
     int valid_args;
     int single_user;
     int single_password;
     char *username;
     char *password;
     char *service;
     char *host;
} args_t;
#endif
