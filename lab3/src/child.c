#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "shared.h"


long long get_line_sum(char* line) {
    long long line_sum = 0;
    int number = 0;
    int digit_power = 1;
    int i = strlen(line);

    while (i >= 0) {
        if ((line[i] >= '0') && (line[i] <= '9')) {
            number += (line[i] - '0') * digit_power;
            digit_power *= 10;
        } else {
            if (line[i] == '-') {
                number *= -1;
            }
            line_sum += number;
            number = 0;
            digit_power = 1;
        }
        i--;
    }

    if (number != 0) {
        line_sum += number;
    }

    return line_sum;
}

int main() {
    int fd = open(SHARED_FILENAME, O_RDWR, 0666); // 0666 — права доступа (чтение и запись для всех)
    if (fd == -1) {
        perror("Ошибка open");
        exit(1);
    }

    SharedMemory *shm = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("Ошибка mmap");
        exit(1);
    }
    close(fd);

    while (1) {
        sem_wait(&shm->sem_parent);

        if (!shm->data_ready) break;

        long long line_sum = get_line_sum(shm->buffer);

        // Обрезаем строку, если она слишком длинная
        size_t len = strnlen(shm->buffer, BUFFER_SIZE);
        if (len > BUFFER_SIZE - 32) len = BUFFER_SIZE - 32;  // Оставляем место под " -> %lld\n"

        char answer[BUFFER_SIZE];
        snprintf(answer, BUFFER_SIZE, "%.*s -> %lld\n", (int)len, shm->buffer, line_sum);

        strncpy(shm->buffer, answer, BUFFER_SIZE);
        shm->buffer[BUFFER_SIZE - 1] = '\0';  // Гарантируем null-терминированную строку
        sem_post(&shm->sem_child);
    }

    munmap(shm, sizeof(SharedMemory));

    return 0;
}
