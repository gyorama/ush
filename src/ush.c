#include <unistd.h>
#include <fcntl.h>
#include "ush.h"
#include <signal.h>
#include <termios.h>

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