#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 500
#define REPEAT 10

void insertion_sort_c(long *arr, int n) {
    for (int i = 1; i < n; i++) {
        long key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

__attribute__((noinline))
void insertion_sort_asm(long *arr, int n) {
    __asm__ volatile (
        "mov $1, %%rcx;"
        "1:;"
        "cmp %[n], %%rcx;"
        "jge 4f;"

        "mov (%%rdi, %%rcx, 8), %%rax;"
        "lea -1(%%rcx), %%rdx;"

        "2:;"
        "cmp $0, %%rdx;"
        "jl 3f;"
        "mov (%%rdi, %%rdx, 8), %%rsi;"
        "cmp %%rax, %%rsi;"
        "jle 3f;"
        "mov %%rsi, 8(%%rdi, %%rdx, 8);"
        "dec %%rdx;"
        "jmp 2b;"

        "3:;"
        "inc %%rdx;"
        "mov %%rax, (%%rdi, %%rdx, 8);"

        "inc %%rcx;"
        "jmp 1b;"

        "4:;"
        :
        : [n]"r"((long)n), "D"(arr)
        : "rax", "rcx", "rdx", "rsi", "memory"
    );
}

int main() {
    srand((unsigned)time(NULL));

    const char *opt = getenv("OPT_LEVEL");
    if (!opt) opt = "O?"; 

    char filename[64];
    snprintf(filename, sizeof(filename), "result_%s.csv", opt);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Cannot open file");
        return 1;
    }

    fprintf(fp, "Size,C_time(ms),ASM_time(ms)\n");

    for (int size = 50; size <= MAX_SIZE; size += 50) {
        double total_c = 0, total_asm = 0;
        for (int r = 0; r < REPEAT; r++) {
            long *arr1 = malloc(size * sizeof(long));
            long *arr2 = malloc(size * sizeof(long));
            if (!arr1 || !arr2) return 1;

            for (int i = 0; i < size; i++) {
                long val = rand() % 100000 - 50000;
                arr1[i] = arr2[i] = val;
            }

            clock_t start, end;

            start = clock();
            insertion_sort_c(arr1, size);
            end = clock();
            total_c += (double)(end - start) / CLOCKS_PER_SEC * 1000;

            start = clock();
            insertion_sort_asm(arr2, size);
            end = clock();
            total_asm += (double)(end - start) / CLOCKS_PER_SEC * 1000;

            free(arr1);
            free(arr2);
        }

        fprintf(fp, "%d,%.6f,%.6f\n", size, total_c / REPEAT, total_asm / REPEAT);
    }

    fclose(fp);
    printf(" %s Completed\n", filename);
    return 0;
}
