// Lots of imports
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <string.h>
#include "ush.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// NOTE: Delete later on 
char **Tracker; // For debugging purposes

// Char-by-char reading
void tty_nonBlocking(int fd) {
    struct termios tty;
    tcgetattr(fd, &tty);
    tty.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(fd, TCSANOW, &tty);
}

// Reset terminal mode to not fuck everything up on other shells
void tty_reset(int fd) {
    struct termios tty;
    tcgetattr(fd, &tty);
    tty.c_lflag |= (ICANON | ECHO);
    tcsetattr(fd, TCSANOW, &tty);
}

// Signal exit
void end() {
    uint16_t i = 0;
    while (Tracker[i] != NULL) {
        printf("%s ", Tracker[i]);
        ++i;
    }    
    tty_reset(STDIN_FILENO);
    exit(0);
}

int main(int argc, char *argv[]) {
    // Signal setup
    signal(SIGINT, end);
    signal(SIGQUIT, end);

    // User setup
    char *name = getenv("USER");
    char *host = getenv("HOSTNAME");
    char dirBuf[4096];

    // Token pointer & (Will need this one later)
    char *token;

    // Exit if error on getting the directory
    if (getcwd(dirBuf, 2048) == NULL) {
        puts("Failed to get current directory.\n");
        perror("Error");
        return 1;
    }

    // Replace nothing with "?"
    if (name == NULL) {
        name = "?";
    } else if (host == NULL) {
        host = "?";
    }

    // Set char-by-char reading
    tty_nonBlocking(STDIN_FILENO);

    // Buffers and indexes
    char cmdBuf[4096];
    char *args[256];
    Tracker = args;
    uint16_t currIndex = 0;
    uint16_t argCount = 0;


    char ch;

    while (1) {
        printf("\033[1m\033[34m%s\033[0m@\033[35m\033[1m%s \033[32m%s\n\033[0m& ", name, host, dirBuf);
        while (ch != '\n') {
            ch = getchar();
            if (ch == '\x7f' || ch == '\b') {
                printf("\033[D \033[D");
                cmdBuf[currIndex] = '\0';
                --currIndex;
            } else if (ch == '\033') {
                getchar(); // Skip the '['
                ch = getchar();
                if (ch != 'A' && ch != 'B') {
                    printf("\033[%c", ch);
                }
            }
            write(STDOUT_FILENO, &ch, 1);
            cmdBuf[currIndex] = ch;
            ++currIndex;
        }
        ch = '\0';

        token = strtok(cmdBuf, " |$;");

        while (token) {
            args[argCount] = token;
            printf("%s ", args[argCount]);
            token = strtok(NULL, " |$;");
            ++argCount;
        }
        token = &cmdBuf[0];
        argCount = 0;
    }

    return 0;
}