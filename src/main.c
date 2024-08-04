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

void tty_nonBlocking(int fd) {
    struct termios tty;
    tcgetattr(fd, &tty);
    tty.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(fd, TCSANOW, &tty);
}


void tty_reset(int fd) {
    struct termios tty;
    tcgetattr(fd, &tty);
    tty.c_lflag |= (ICANON | ECHO);
    tcsetattr(fd, TCSANOW, &tty);
}

void end() {
    tty_reset(STDIN_FILENO);
    exit(0);
}

int main(int argc, char *args[]) {
    signal(SIGINT, end);
    signal(SIGQUIT, end);
    char *name = getenv("USER");
    char *host = getenv("HOSTNAME");
    char dirBuf[2048];

    char *token;

    if (getcwd(dirBuf, 2048) == NULL) {
        puts("Failed to get current directory.\n");
        perror("Error");
        return 1;
    }

    if (name == NULL) {
        name = "?";
    } else if (host == NULL) {
        host = "?";
    }

    tty_nonBlocking(STDIN_FILENO);

    char cmdBuf[4096];
    uint16_t currIndex = 0;

    char ch;

    while (1) {
        printf("\033[1m\033[34m%s\033[0m@\033[35m\033[1m%s \033[32m%s\n\033[0m& ", name, host, dirBuf);
        while (ch != '\n') {
            ch = getchar();
            if (ch == '\x7f' || ch == '\b') {
                printf("\033[D \033[D");
                --currIndex;
            } else if (ch == '\033') { // Check for escape sequence (beginning of arrow key codes)
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
            printf("%s ", token);
            token = strtok(NULL, " |$;");
        }
        token = &cmdBuf[0];
    }


    return 0;
}