#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define BUFFER_SIZE 10

int main() {
    char message[BUFFER_SIZE] = "Hi There";
    char received_message[BUFFER_SIZE];
    int fd[2];
    pipe(fd);
    int pid = fork();
    if (pid == 0) {
        close(fd[1]);
        read(fd[0], received_message, sizeof(received_message));
        close(fd[0]);
        for (int i = 0; i < strlen(received_message); ++i) {
            if (islower(received_message[i])) {
                received_message[i] = toupper(received_message[i]);
                continue;
            }
            received_message[i] = tolower(received_message[i]);
        }
        printf("%s", received_message);
    } else {
        close(fd[0]);
        write(fd[1], message, sizeof(message));
        close(fd[1]);
    }
    return 0;
}