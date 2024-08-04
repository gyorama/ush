#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "ush.h"
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

void runCommand(const char *argv[]) {
    pid_t pid = fork();

    if (pid == 0) {
        signal(SIGINT, exit);
        if (execvp(argv[0], (char * const *)argv) == -1) {
            perror(argv[0]);
            exit(1);
        }
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    } else if (pid == -1) {
        perror(argv[0]);
        exit(1);
    }
}

void quit(const char *argv[]) {
    exit(atoi(argv[1]));
}

int fileWrite(const char *argv[]) {
    int writeFile = 0;
    for (int i = 1; argv[i] != NULL; i++) {
        if (strcmp(argv[i], ">") == 0) {
            argv[i] = NULL;
            writeFile = open(argv[i + 1], O_WRONLY| O_CREAT | O_TRUNC, 0644);
            int standardOut = dup(STDOUT_FILENO);
            if (dup2(writeFile, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }
            return standardOut;
        }
    }

    return 0;
}

void checkForEnvironmentVariables(const char *argv[]) {
    for (int i = 1; argv[i] != NULL; i++) {
        if (argv[i][0] == '$') {
            argv[i]++;
            char *environmentVariable = getenv(argv[i]);

            if (environmentVariable != NULL) {
                argv[i] = environmentVariable;
                return;
            } else {
                argv[i] = "";
            }
        }
    }
}

void translateSymbols(const char *argv[]) {
    for (int i = 0; argv[i] != NULL; i++) {
        switch (argv[i][0]) {
        case '~':
            break;

        case '*':
            argv[i] = getenv("PWD");
            break;
        
        default:
            break;
        }
    }
    
}