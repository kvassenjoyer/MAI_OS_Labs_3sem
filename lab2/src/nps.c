#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "unistd.h"

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

long long found = 0;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int required;
} barrier_t;

void barrier_init(barrier_t *barrier, int required) {
    pthread_mutex_init(&barrier->mutex, NULL);
    pthread_cond_init(&barrier->cond, NULL);
    barrier->count = 0;
    barrier->required = required;
}

void barrier_wait(barrier_t *barrier) {
    pthread_mutex_lock(&barrier->mutex);
    barrier->count++;
    if (barrier->count == barrier->required) {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
    } else {
        pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }
    pthread_mutex_unlock(&barrier->mutex);
}

void barrier_free(barrier_t *barrier) {
    barrier->count = 0;
    pthread_cond_broadcast(&barrier->cond);
    pthread_mutex_unlock(&barrier->mutex);
}

typedef struct {
    char *str;
    char *substr;
    long long left;
    long long max_threads;
    long long nps_result;
} ThreadData;

barrier_t barrier;

long long find_substr(char *str, char *substr, long long left, long long max_threads) {
    long long str_len = strlen(str);
    long long substr_len = strlen(substr);
    long long result = -1;
    long long j = 0;
    for (long long i = left; i < str_len; i += max_threads) {
        if (found) {
            break;
        }

        if (i + j > str_len) {
            break;
        }

        for (j = 0; j < substr_len; j++) {
            if (substr[j] != str[i + j]) {
                break;
            }
        }
        if (j == substr_len) {
            found = 1;
            pthread_cond_broadcast(&barrier.cond);
            return i;
        }
        barrier_wait(&barrier);
    }
    return result;
}

void *npsThread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    long long nps_result = find_substr(data->str, data->substr, data->left, data->max_threads);
    data->nps_result = nps_result;
    pthread_exit(NULL);
}

long long nps(char *str, char *substr, long long max_threads) {
    barrier_init(&barrier, max_threads);
    pthread_t *threads = (pthread_t *)malloc(max_threads * sizeof(pthread_t));
    ThreadData *threadData = (ThreadData *)malloc(max_threads * sizeof(ThreadData));

    size_t str_len = strlen(str);
    size_t substr_len = strlen(substr);

    for (long long i = 0; i < max_threads; i++) {
        threadData[i].str = str;
        threadData[i].substr = substr;
        threadData[i].left = i;
        threadData[i].max_threads = max_threads;
        threadData[i].nps_result = -1;
    }
    for (long long i = 0; i < max_threads; i++) {
        pthread_create(&threads[i], NULL, npsThread, (void *)&threadData[i]);
    }

    long long nps_result = str_len;
    for (long long i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
        if ((threadData[i].nps_result != -1) && (threadData[i].nps_result < nps_result)) {
            nps_result = threadData[i].nps_result;
        }
    }

    if (nps_result == str_len) {
        nps_result = -1;
    }

    free(threads);
    free(threadData);
    barrier_free(&barrier);
    return nps_result;
}

void timer_nps_launch(char *str, char *substr, long long max_threads) {
    clock_t start = clock();
    long long nps_result = nps(str, substr, max_threads);
    clock_t end = clock();
    double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Подстрока впервые встречается на индексе: %lld\n", nps_result);
    printf("Время затраченное на поиск подстроки: %.6f секунд\n", elapsed_time);
    printf("Максимальное количество потоков: %lld\n\n", max_threads);
    found = 0;
}

long long main(long long argc, char *argv[]) {
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

    printf("Результат однопоточной strstr: %ld \n\n", (MAX(-1, strstr(str, substr) - str)));

    timer_nps_launch(str, substr, max_threads);

    return 0;
}
