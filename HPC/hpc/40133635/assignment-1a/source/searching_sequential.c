
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <float.h>

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



int hostMatch(long *comparisons)
{
	int i,j,k, lastI;
	
	i=0;
	j=0;
	k=0;
	lastI = textLength-patternLength;
        *comparisons=0;

	while (i<=lastI && j<patternLength)
	{
                (*comparisons)++;
		if (textData[k] == patternData[j])
		{
			k++;
			j++;
		}
		else
		{
			i++;
			k=i;
			j=0;
		}
	}
	if (j == patternLength)
		return i;
	else
		return -1;
}
void processData()
{
	unsigned int result;
        long comparisons;

	printf ("Text length = %d\n", textLength);
	printf ("Pattern length = %d\n", patternLength);

	result = hostMatch(&comparisons);
	if (result == -1)
		printf ("Pattern not found\n");
	else
		printf ("Pattern found at position %d\n", result);
        printf ("# comparisons = %ld\n", comparisons);

}

int main(int argc, char **argv)
{
        uint64_t diff, diff1; 
	struct timespec start, end , clock1 , clock2; 
        int i;
	double seconds1, seconds2;
 	int Digs = DECIMAL_DIG;

	int testNumber;
	testNumber = 0;
	while (readData (testNumber))
	{
		clock_gettime(CLOCK_MONOTONIC, &start); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &clock1);	
   	 	processData();
		clock_gettime(CLOCK_MONOTONIC, &end); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &clock2);
                diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		seconds1 = (double)diff / 1000000000.0;
	        printf("elapsed time = %.*e Seconds\n", Digs, seconds1);
		diff1 = BILLION * (clock2.tv_sec - clock1.tv_sec) + clock2.tv_nsec - clock1.tv_nsec;
		seconds2 = (double)diff1 / 1000000000.0;
		printf("elapsed process CPU time = %.*e Seconds\n\n", Digs, seconds2);
		testNumber++;
	}


}
