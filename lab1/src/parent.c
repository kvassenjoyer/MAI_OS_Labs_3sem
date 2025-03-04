#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

#define MAX_FILENAME 256
#define BUFFER_SIZE 4096

int main(){
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
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        execl("./child", "child", NULL);
        perror("Ошибка execl для child\n");
        exit(1);
    }

    close(pipe1[0]);

    char filepath[MAX_FILENAME];
    printf("Введите путь к файлу:\n");
    scanf("%s", filepath);
    
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        printf("Ошибка open\n");
        exit(1);
    }
    
    
    char filedata[BUFFER_SIZE];
    read(fd, filedata, BUFFER_SIZE);
    write(pipe1[1], filedata, strlen(filedata));

    close(fd);
    close(pipe1[1]);

    wait(NULL);

    return 0;
}

