
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <omp.h>

int main( int argc, char *argv[])
{
	int nthreads , tid;
	int c[100];
	int a[100] = {[0 ... 99] = 1} ;
	int b[100] = {[0 ... 99] = 1};
	#pragma omp parallel for schedule(static)num_threads(2)
		for(int i=0;i<100; i++){
			c[i] = a[i] + b[i];
			printf("C[i] values : %d \n " , c[i]);
		}
}

