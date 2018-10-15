
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>




////////////////////////////////////////////////////////////////////////////////
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
    int i = 0, j = 0, k = 0, lastI = textLength-patternLength;
    long comparisons_tmp = 0;
    int found = textLength + 1;
    int breaking = 0;

    #pragma omp parallel for reduction(+:comparisons_tmp) schedule(static) num_threads(4) default(none) shared(found, comparisons) private(j, k) firstprivate(lastI, textData, patternData, patternLength, breaking)
    for(i = 0; i<= lastI; i++)
    {
         if(i < found)
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
                       found = i;

            }
	    else
		breaking=0;
         }
    }
    *comparisons = comparisons_tmp;
    if(found < textLength + 1){
         return found;
    }
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
	int testNumber;
	if(argc == 1){
		testNumber = 0;
		while (readData (testNumber))
		{
			c0 = clock(); t0 = time(NULL);	
   	 		processData();
			c1 = clock(); t1 = time(NULL);
                	printf("Test %d elapsed wall clock time = %ld\n", testNumber, (long) (t1 - t0));
                	printf("Test %d elapsed CPU time = %f\n\n", testNumber, (float) (c1 - c0)/CLOCKS_PER_SEC); 
			testNumber++;
		}
	}
	if(argc == 2){
		int tester = atoi(argv[1]);
		readData(tester);
		c0 = clock(); t0 = time(NULL);
                processData();
                c1 = clock(); t1 = time(NULL);
                printf("Test %d elapsed wall clock time = %ld\n", testNumber, (long) (t1 - t0));
                printf("Test %d elapsed CPU time = %f\n\n", testNumber, (float) (c1 - c0)/CLOCKS_PER_SEC);
	}
}
