#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <omp.h>


int main(int argc, char *argv[])
{
    int sum_local =0;
    int sum = 0;
    #pragma omp parallel num_threads(8) reduction(+: sum)
    {
    int ThreadNum = omp_get_thread_num();
    sum_local = ThreadNum;
    sum_local += 5;
    }
    sum =sum_local; 
    printf("Sum : %d\n" , sum);

}
