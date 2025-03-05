#include "stdio.h"
#include "string.h"
#include "unistd.h"

#define BUFFER_SIZE 4096

long long get_line_sum(char* line) {
    // можно и strol использовать, если быть особо ленивым
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
    char line[BUFFER_SIZE];

    while (fgets(line, BUFFER_SIZE, stdin) != NULL) {
        long long line_sum = get_line_sum(line);

        write(STDOUT_FILENO, line, strlen(line) - 1);
        printf(" -> %lld\n", line_sum);
    }

    return 0;
}