#define _CRT_RAND_S
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

struct response {
    int index;
    double duration;
};

void search(
    const unsigned int* array,
    const unsigned int target,
    const int count,
    struct response* resp)
{
    double start, end;
    int index = -1;

    start = omp_get_wtime();
    for (int i = 0; i < count; i++)
    {
        if (array[i] == target)
        {
            index = i;
            break;
        }
    }
    end = omp_get_wtime();

    resp->index = index;
    resp->duration = end - start;
}

int main(int argc, char** argv)
{
    const int count = 1000000000;
    unsigned int target;
    unsigned int* array;

    struct response resp;

    /* Generate the random array */
    array = (unsigned int*)malloc(count*sizeof(unsigned int));
    for(int i = 0; i < count; ++i) rand_s(array + i);

    for (int i = 0; i < 20; ++i) {
        rand_s(&target);
        search(array, target, count, &resp);
        printf("____________________\n");
        if (resp.index >= 0) printf("Found occurence of %u at index %d;\n", target, resp.index);
        else printf("Number %u was not found;\n", target);
        printf("Time spent: %f\n", resp.duration);
    }

    return(0);
}