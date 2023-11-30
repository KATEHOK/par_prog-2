#define _CRT_RAND_S
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

struct response {
    int index;
    double duration;
};

void search(
    const int threads,
    const unsigned int* array,
    const unsigned int target,
    const int count,
    struct response* resp)
{
    double start, end;
    int i, index = count;
    start = omp_get_wtime();

    #pragma omp parallel num_threads(threads) reduction(min: index) private(i)
    {
        #pragma omp for
        for (i = 0; i < count; i++)
        {
            if (array[i] == target)
            {
                index = i;
                break;
            }
        }
    }

    end = omp_get_wtime();
    resp->index = index;
    resp->duration = end - start;
}

int main(int argc, char** argv)
{
    const int count = 1000000000;
    const int targets_count = 50;
    int available_threads;
    unsigned int* array;
    unsigned int* targets;
    struct response resp;
    FILE* file;
    errno_t status = fopen_s(&file, "../par_prog-2/report/data.txt", "w");
    if (status != 0) {
        printf("File was not opened!\nError code: %d\n", status);
        return 0;
    }

    available_threads = omp_get_num_procs();
    printf("OpenMP: %d; Threads: %d\n", _OPENMP, available_threads);

    /* Generate random array and targets*/
    array = (unsigned int*)malloc(count*sizeof(unsigned int));
    targets = (unsigned int*)malloc(targets_count * sizeof(unsigned int));
    for (int i = 0; i < count || i < targets_count; ++i) {
        if (i < count) rand_s(array + i);
        if (i < targets_count) rand_s(targets + i);
    }

    // For all counts of threads
    for (int threads = 1; threads <= available_threads; ++threads) {

        status = fprintf(file, "\nThreads: %d\n#\tTIME\tFOUND\n", threads);
        if (status < 0) printf("Data was not written!\nError code: %d\n", status);

        // For all counts of targets
        for (int i = 0; i < targets_count; ++i) {
            int found = 0;

            // main part
            search(threads, array, targets[i], count, &resp);

            // std output
            printf("[%d:%d]____________________\n", threads, i + 1);
            if (resp.index < count) {
                printf("%u FOUND at %d;\n", targets[i], resp.index);
                ++found;
            }
            else printf("%u NOT FOUND;\n", targets[i]);
            printf("TIME: %f\n\n", resp.duration);

            // file output
            status = fprintf(file, "%d\t%f\t%d\n", i + 1, resp.duration, found);
            if (status < 0) printf("Data was not written!\nError code: %d\n", status);
        }
    }

    free(array);
    free(targets);
    fclose(file);
    return(0);
}