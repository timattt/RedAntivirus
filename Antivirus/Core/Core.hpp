#pragma once

// includes
#include <map>
#include <set>
#define _POSIX_C_SOURCE 200809L
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/signalfd.h>
#include <fcntl.h>

// defines
#define ENSURE_NONNULL(X) if ((X) == NULL) {printf("Something is null in %s at %d line.\n", __FILE__, __LINE__); perror(""); exit(-1);}
#define ENSURE(X, ERRMES) if (!(X)) {printf("Something went wrong: [%s] in %s at %d line.\n", ERRMES, __FILE__, __LINE__); perror(""); exit(-1);}

// constants
#define FLUSH_TIME 1000
#define OLD_LISTENING_TIME 10000

// system observer
void initObserver(const char * dir);
void updateObserver();

// core
void runObserver(const char * dir);

// analyzer
void allowKills();
void reportAction(int pid, int fd);
void parseOlds();
void tryToPrintInfo();

// utils
char* get_program_name_from_pid(int pid, char *buffer, size_t buffer_size);
long long getCurrentMillis();
void fansyPrint(const char * mes, int count);

// colors
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
