// question: what is the issue here???

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
    int arr[] = {1, 2, 3, 4, 1, 2};
    int arr_size = sizeof(arr) / sizeof(int);
    int start, end;
    int fd[2];
    if (pipe(fd) == -1) return 1;
    int pid = fork();

    if (pid == 0){
        start = 0;
        end = arr_size / 2;
    } else {
        start = arr_size / 2;
        end = arr_size;
    }

    // calculation:
    int sum = 0;
    for (int i = start ; i < end; i++) {
        sum += arr[i];
    }
    printf("The sum is: %d\n", sum);

    // transferring the value;
    if (pid == 0) {
        close(fd[0]);
        if(write(fd[1], &sum, sizeof(sum)) == -1) return 1;
        close(fd[1]);
    } else {
        int partial_sum;
        close(fd[1]);
        if (read(fd[0], &partial_sum, sizeof(partial_sum)) == -1) return 2;
        close(fd[0]);
        int result = partial_sum + sum;
        printf("The final result is: %d", result);
        wait(NULL);
    }
}