#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

typedef struct {
    char *str;
    char *substr;
    int left;
    int right;
    int nps_result;
} ThreadData;

int find_substr(char *str, char *substr, int left, int right) {
    int str_len = strlen(str);
    int substr_len = strlen(substr);
    for (int i = left; i <= right; i++) {
        int j;
        for (j = 0; j < substr_len; j++) {
            if (i + j >= str_len) {
                break;
            }
            if (substr[j] != str[i + j]) {
                break;
            }
        }
        if (j == substr_len) {
            return i;
        }
    }
    return -1;
}

void *npsThread(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    int nps_result = find_substr(data->str, data->substr, data->left, data->right);
    data->nps_result = nps_result;

    pthread_exit(NULL);
}

int nps(char *str, char *substr, int max_threads) {
    pthread_t *threads = (pthread_t *)malloc(max_threads * sizeof(pthread_t));
    ThreadData *threadData = (ThreadData *)malloc(max_threads * sizeof(ThreadData));

    size_t str_len = strlen(str);
    size_t substr_len = strlen(substr);

    int chunkSize = MAX(substr_len, str_len / max_threads);
    int used_threads = 0;
    for (int i = 0; i < max_threads; i++) {
        int start = i * chunkSize;
        int end = MIN(start + chunkSize - 1, str_len - 1);

        if (start < str_len) {
            used_threads++;
            threadData[i].str = str;
            threadData[i].substr = substr;
            threadData[i].left = start;
            threadData[i].right = end;
            threadData[i].nps_result = -1;
            pthread_create(&threads[i], NULL, npsThread, (void *)&threadData[i]);
        } else {
            break;
        }
    }

    int nps_result = str_len;
    for (int i = 0; i < used_threads; i++) {
        pthread_join(threads[i], NULL);

        if ((threadData[i].nps_result != -1) && (threadData[i].nps_result < nps_result)) {
            nps_result = threadData[i].nps_result;
        }
        printf("| %d\t |%d\t |%d\t |%d |\n", i, threadData[i].left, threadData[i].right,
               threadData[i].nps_result);
    }
    if (nps_result == str_len) {
        nps_result = -1;
    }

    free(threads);
    free(threadData);

    return nps_result;
}

void timer_nps_launch(char *str, char *substr, int max_threads) {
    clock_t start = clock();
    int nps_result = nps(str, substr, max_threads);
    clock_t end = clock();
    double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Подстрока впервые встречается на индексе: %d\n", nps_result);
    printf("Время затраченное на поиск подстроки: %.6f секунд\n", elapsed_time);
    printf("Максимальное количество потоков: %d\n\n", max_threads);
}

int main(int argc, char *argv[]) {
    // NPS - native pattern search
    if (argc < 2) {
        fprintf(stderr, "Ошибка: не указано количество потоков\n");
        return 1;
    }

    int max_threads = atoi(argv[1]);

    if (max_threads < 1) {
        fprintf(stderr, "Ошибка: некорректное количество потоков\n");
        return 1;
    }

    int str_len;
    printf("Введите длину строки: ");
    scanf("%d", &str_len);

    if (str_len < 1) {
        fprintf(stderr, "Ошибка: некорректная длина строки\n");
        return 1;
    }

    char *str = (char *)malloc(str_len * sizeof(char));

    srand(time(NULL));

    int custom_substr_len = 0;
    int substr_len = MAX(1, MIN(str_len, custom_substr_len));
    int substr_end = substr_len + rand() % (MAX(1, str_len - substr_len));
    int substr_start = substr_end - substr_len;
    printf("\nДлины строки и подстроки: %d %d\n\n", str_len, substr_len);

    char *substr = (char *)malloc(substr_len * sizeof(char));

    for (int i = 0; i < str_len; i++) {
        str[i] = (rand() % 96) + ' ';
    }

    for (int i = 0; i < substr_len; i++) {
        substr[i] = str[substr_start + i];  // Подстрока всегда должна найтись
        // substr[i] = (rand() % 96) + ' ';  // Подстрока может и не найтись (-1)
    }

    clock_t start, end;
    int nps_result;
    double elapsed_time;

    timer_nps_launch(str, substr, max_threads);
    printf("Результат через однопоточной strstr: %ld \n", (MAX(-1, strstr(str, substr) - str)));

    return 0;
}
