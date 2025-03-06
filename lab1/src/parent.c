#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

#define MAX_FILEPATH 256
#define BUFFER_SIZE 4096

int main() {
    char filepath[MAX_FILEPATH];
    printf("Введите путь к файлу:\n");
    scanf("%s", filepath);

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        printf("Ошибка open\n");
        exit(1);
    }

    int pipe1[2];
    pid_t child;

    if (pipe(pipe1) == -1) {
        perror("Ошибка при создании канала\n");
        exit(1);
    }

    if ((child = fork()) == -1) {
        perror("Ошибка при создании дочернего процесса\n");
        exit(1);
    }

    if (child == 0) {
        close(pipe1[0]);
        dup2(fd, STDIN_FILENO);
        dup2(pipe1[1], STDOUT_FILENO);
        close(fd);
        close(pipe1[1]);
        execl("./child", "child", NULL);
        perror("Ошибка execl для child\n");
        exit(1);
    }

    close(fd);
    close(pipe1[1]);
    dup2(pipe1[0], STDIN_FILENO);
    close(pipe1[0]);

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, stdin) != NULL) {
        write(STDOUT_FILENO, line, strlen(line));
    }

    wait(NULL);

    return 0;
}
