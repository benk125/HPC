
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <stdint.h>
#include <unistd.h>
#include <float.h>
#include <argp.h>
#include <sys/stat.h>

#define BILLION 1000000000L 

int localpid(void) 
{ 
	static int a[9] = { 0 }; 
	return a[0]; 
}

// Program main
////////////////////////////////////////////////////////////////////////////////

char *textData;
int textLength;

char *patternData;
int patternLength;

clock_t c0, c1;
time_t t0, t1;

void outOfMemory()
{
	fprintf (stderr, "Out of memory\n");
	exit (0);
}

void readFromFile (FILE *f, char **data, int *length)
{
	int ch;
	int allocatedLength;
	char *result;
	int resultLength = 0;

	allocatedLength = 0;
	result = NULL;

	

	ch = fgetc (f);
	while (ch >= 0)
	{
		resultLength++;
		if (resultLength > allocatedLength)
		{
			allocatedLength += 10000;
			result = (char *) realloc (result, sizeof(char)*allocatedLength);
			if (result == NULL)
				outOfMemory();
		}
		result[resultLength-1] = ch;
		ch = fgetc(f);
	}
	*data = result;
	*length = resultLength;
}

int readData (int testNumber)
{
	FILE *f;
	char fileName[1000];
#ifdef DOS
        sprintf (fileName, "..\\data\\test%d\\text.txt", testNumber);
#else
	sprintf (fileName, "../data/test%d/text.txt", testNumber);
#endif
	f = fopen (fileName, "r");
	if (f == NULL)
		return 0;
	readFromFile (f, &textData, &textLength);
	fclose (f);
#ifdef DOS
        sprintf (fileName, "..\\data\\test%d\\pattern.txt", testNumber);
#else
	sprintf (fileName, "../data/test%d/pattern.txt", testNumber);
#endif
	f = fopen (fileName, "r");
	if (f == NULL)
		return 0;
	readFromFile (f, &patternData, &patternLength);
	fclose (f);

	printf ("Read test number %d\n", testNumber);

	return 1;

}



int hostMatch(long *comparisons, int numThreads)
{
	int i = 0, j = 0, k = 0,lastI = textLength-patternLength;
	int position = textLength +1;;
	long comparisons_tmp = 0;
        int breaking = 0;
	lastI = textLength-patternLength;
        #pragma omp parallel num_threads(numThreads) private(k,j) shared(position, comparisons) reduction(+: comparisons_tmp) firstprivate(breaking, lastI , textData, patternData, patternLength)	
	#pragma omp for schedule(runtime)
	 for(i = 0; i<= lastI; i++)
   	 {
             if(i < position)
             {
                 k=i; j=0;
                while(j < patternLength && breaking == 0){
                    comparisons_tmp++;
                    if(textData[k] == patternData[j])
                    {
                        k++;
                        j++;
                    }
                    else
                    {
                        breaking=1;
                    }
                }
                if(j == patternLength)
                {
                       position = i;
                }
                else{
                    breaking=0;
		}
              }
    	}
    *comparisons = comparisons_tmp;
    if(position < textLength + 1){
         return position;
    }
    else
         return -1;

		    
}


void processData(int numThreads)
{
	unsigned int result;
        long comparisons;

	printf ("Text length = %d\n", textLength);
	printf ("Pattern length = %d\n", patternLength);

	result = hostMatch(&comparisons, numThreads);
	if (result == -1)
		printf ("Pattern not found\n");
	else
		printf ("Pattern found at position %d\n", result);
        printf ("# comparisons = %ld\n", comparisons);

}

struct passedargs
{
    int testNumber;
    int NumThreads;
};

int checkTestNumber(int testNumber)
{
    if(testNumber < 3 && testNumber >=0)
    {
	return testNumber;
    }
    if(testNumber > 3 || testNumber <0)
    {
	printf("Invalid TestNumber Please Use Test Case value between 0 - 3\n");
	exit(EXIT_FAILURE);
    }
    else
    {
	printf("Performing Pattern search on all Test Cases \n");
        return -1;	
    }
}

int checkThreadNumber( int numThreads)
{
    if(numThreads < 0 || numThreads > 16)
    {
	printf("Please use a suitable Number of threads\n");
	exit(EXIT_FAILURE);
    }
    printf("Using %d Thread/s \n" , numThreads);
    return numThreads;
}


int main(int argc, char **argv)
{
        uint64_t diff, diff1; 
	struct timespec start, end , clock1 , clock2; 
	double seconds1, seconds2;
 	int Digs = DECIMAL_DIG;
	int numThreads = 2;
	int testNumber;
	int tester = 1;

	if(argc == 2)
	{
	    tester = checkTestNumber(atoi(argv[1]));
	}
	if(argc == 3)
	{
	    tester = checkTestNumber(atoi(argv[1]));
	    numThreads = checkThreadNumber(atoi(argv[2]));
	}
	
        if( argc == 1)
	{
	    testNumber = 0;
	    while (readData (testNumber))
	    {
		clock_gettime(CLOCK_MONOTONIC, &clock1); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);	
   	 	processData(numThreads);
		clock_gettime(CLOCK_MONOTONIC, &clock2); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
                diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		seconds1 = (double)diff / 1000000000.0;
	        printf("elapsed time = %.*e Seconds\n", Digs, seconds1);
		diff1 = BILLION * (clock2.tv_sec - clock1.tv_sec) + clock2.tv_nsec - clock1.tv_nsec;
		seconds2 = (double)diff1 / 1000000000.0;
		printf("elapsed process CPU time = %.*e Seconds\n\n", Digs, seconds2);
		testNumber++;
	    }
	}
	else
	{	
		readData(tester);
	        clock_gettime(CLOCK_MONOTONIC, &clock1); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
                processData(numThreads);
                clock_gettime(CLOCK_MONOTONIC, &clock2); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
                diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
                seconds1 = (double)diff / 1000000000.0;
                printf("elapsed time = %.*e Seconds\n", Digs, seconds1);
                diff1 = BILLION * (clock2.tv_sec - clock1.tv_sec) + clock2.tv_nsec - clock1.tv_nsec;
                seconds2 = (double)diff1 / 1000000000.0;
                printf("elapsed process CPU time = %.*e Seconds\n\n", Digs, seconds2);

	}

}
