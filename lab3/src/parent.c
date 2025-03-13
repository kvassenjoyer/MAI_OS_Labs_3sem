#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shared.h"


int main() {
    char filepath[BUFFER_SIZE];
    printf("Введите путь к файлу:\n");
    scanf("%s", filepath);

    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Ошибка fopen");
        exit(1);
    }

    int fd = open(SHARED_FILENAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка open");
        exit(1);
    }
    ftruncate(fd, sizeof(SharedMemory));

    SharedMemory *shm = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("Ошибка mmap");
        exit(1);
    }
    close(fd);

    sem_init(&shm->sem_parent, 1, 0);
    sem_init(&shm->sem_child, 1, 1);

    pid_t child = fork();
    if (child == -1) {
        perror("Ошибка fork");
        exit(1);
    }

    if (child == 0) {
        execl("./child", "child", NULL);
        perror("Ошибка execl");
        exit(1);
    }

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file)) {
        sem_wait(&shm->sem_child);
        strncpy(shm->buffer, line, BUFFER_SIZE);
        shm->data_ready = 1;
        sem_post(&shm->sem_parent);

        sem_wait(&shm->sem_child);
        printf("%s", shm->buffer);
        sem_post(&shm->sem_parent);
    }

    sem_wait(&shm->sem_child);
    shm->data_ready = 0;
    sem_post(&shm->sem_parent);

    wait(NULL);

    sem_destroy(&shm->sem_parent);
    sem_destroy(&shm->sem_child);
    munmap(shm, sizeof(SharedMemory));
    remove(SHARED_FILENAME);
    fclose(file);

    return 0;
}
