#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    char *str;
    char *substr;
    long long start;
    long long end;
    long long result;
    pthread_mutex_t *mutex;
    long long *found;
} ThreadData;

void *find_substr(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    size_t substr_len = strlen(data->substr);

    for (long long i = data->start; i <= data->end - substr_len; i++) {
        if (*data->found != -1) break;
        
        if (strncmp(&data->str[i], data->substr, substr_len) == 0) {
            pthread_mutex_lock(data->mutex);
            if (*data->found == -1 || i < *data->found) {
                *data->found = i;
            }
            pthread_mutex_unlock(data->mutex);
            break;
        }
    }
    return NULL;
}

long long parallel_search(char *str, char *substr, long long max_threads) {
    size_t str_len = strlen(str);
    size_t substr_len = strlen(substr);
    if (str_len < substr_len) return -1;

    pthread_t threads[max_threads];
    ThreadData threadData[max_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    long long found = -1;

    long long chunk_size = (str_len - substr_len + 1) / max_threads;
    chunk_size = MAX(1, chunk_size);

    for (long long i = 0; i < max_threads; i++) {
        threadData[i].str = str;
        threadData[i].substr = substr;
        threadData[i].start = i * chunk_size;
        threadData[i].end = MIN((i + 1) * chunk_size, str_len - substr_len + 1);
        threadData[i].result = -1;
        threadData[i].mutex = &mutex;
        threadData[i].found = &found;
        pthread_create(&threads[i], NULL, find_substr, &threadData[i]);
    }

    for (long long i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return found;
}

void timer_search(char *str, char *substr, long long max_threads) {
    printf("Количество потоков: %lld\n", max_threads);
    clock_t start = clock();
    long long result = parallel_search(str, substr, max_threads);
    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Подстрока впервые встречается на индексе: %lld\n", result);
    printf("Время затраченное на поиск подстроки: %.6f секунд\n\n", elapsed_time);
}

int main(int argc, char *argv[]) {
    // NPS - native pattern search
    if (argc < 2) {
        fprintf(stderr, "Ошибка: не указано количество потоков\n");
        return 1;
    }

    long long max_threads = atoi(argv[1]);

    if (max_threads < 1) {
        fprintf(stderr, "Ошибка: некорректное количество потоков\n");
        return 1;
    }

    long long str_len;
    printf("Введите длину строки: ");
    scanf("%lld", &str_len);

    if (str_len < 1) {
        fprintf(stderr, "Ошибка: некорректная длина строки\n");
        return 1;
    }

    char *str = (char *)malloc(str_len * sizeof(char));

    srand(time(NULL));

    long long custom_substr_len = 100;
    long long substr_len = MAX(1, MIN(str_len, custom_substr_len));
    long long substr_end = (str_len - str_len / 10) + rand() % (MAX(1, str_len / 10));
    long long substr_start = substr_end - substr_len;

    printf("\nДлины строки и подстроки: %lld %lld\n\n", str_len, substr_len);

    char *substr = (char *)malloc(substr_len * sizeof(char));

    for (long long i = 0; i < str_len; i++) {
        str[i] = (rand() % 96) + ' ';
    }

    for (long long i = 0; i < substr_len; i++) {
        substr[i] = str[substr_start + i];  // Подстрока всегда должна найтись
        // substr[i] = (rand() % 96) + ' ';  // Подстрока может и не найтись (-1)
    }

    printf("Результат однопоточной strstr: %ld \n\n", (long)(strstr(str, substr) - str));
    
    for (long long threads = 1; threads <= max_threads; threads *= 2) {
        timer_search(str, substr, threads);
    }

    free(str);
    free(substr);
    return 0;
}