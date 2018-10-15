/*------------------------------------------------------------------------------
 This program changes the sequential algorithm into a more parallel search 
 algorithm using opemp.

 This algorithm improves upon that of searching_OMP_0, as we can assume that there
 is only one occurence of the pattern, as soon as a match is found a shared
 flag variable will be set, every other for loop wishing to get a new i value to check
 for a pattern will be stopped from doing more comparisons. In newer versions of openmp,
 this could be achieved with pragma omp cancel
 
 1. To run
    $./searching_OMP_1
    With no flags specified this will run the algorithm against all test cases
    in the input folder
    
    Flags
    -n      Specifies the test number to run the algorithm against
    -t      Specifies the number of threads to run the algorithm with
 
 2. Example with flags
    $./searching_OMP_1 -n 1 -t 2
    This runs the algorithm against test 1 with 2 threads.
 * ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>
#include <stdint.h>
#include <float.h>
#include <argp.h>
#include <sys/stat.h>

#define BILLION 1000000000L 

int localpid(void)
{
        static int a[9] = { 0 };
        return a[0];
}


////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////

char *textData;
int textLength;

char *patternData;
int patternLength;

clock_t c0, c1;
time_t t0, t1;

//The struct containing the two possible flag inputs
struct genargs{
    int testNum;
    int numofthreads;
};

//The help options that will appear if the --help or -h flag is specified
static struct argp_option options[]={
    {"testNum", 'n', "testNum", 0, "\tTest Number to run sequential search on"},
    {"threads", 't', "numofthreads", 0, "\tNumber of threads to use for algorithm"},
    {0},
};

static char args_doc[] = "Runs the algorithm against a specific search. If \
no flag is specified then the algorithm will be ran on all tests in the input folder";

//Switch case for the different arguments. Flag n for the test number and
//flag t for the number of threads
int parse_opt(int key, char *arg, struct argp_state *state){
    struct genargs *arguments = (*state).input;

    switch (key)
    {
        case ('n'):
            (*arguments).testNum = atoi(arg);
            break;
        case ('t'):
            (*arguments).numofthreads = atoi(arg);
            break;
        default:
            break;
}
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, " "};

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


/* The parallelised algorithm showing the private variables for each thread (i and j)
 * and the shared variable (pos and flag) which will be common amongst all threads.
 * The number of threads is also passed in here 
 *
 * Reduction variable comps set to add up all of the comparisons once the threads have
 * finished executing
 *
 * Scheduler set to runtime so that it can be ran later
 *
 * The method will take each character position and and compare the text data to the 
 * pattern data, If at the end there is a match then the position will be recorded and
 * returned, each time a char is compared the comp variable will be incremented.
 * 
 * As we are assuming there is only one occurrence we can improve upon the previous
 * algorithm by setting a flag once the first match has been found (as there should be 
 * no other) The flag wills stop all other threads from entering the while loop on a new
 * i iteration. As we are assuming one occurence, only one thread should be setting the flag
 * variable so no critical section is needed
 * */


int hostMatch(long *comparisons, int numofthreads)
{
	int pos, i,j, lastI;
    int volatile flag=0;
	long comps = 0;
	pos = -1;
	lastI = textLength-patternLength;
    *comparisons=0;

    #pragma omp parallel private(i,j) shared(pos,flag) num_threads(numofthreads) reduction(+ : comps)
    #pragma omp for schedule(runtime)
	for(i=0; i<=lastI; i++)
	{
    if(flag!=1){ //Has a match been found
        j=0;
        while(j<patternLength && textData[i+j] == patternData[j] && pos==-1)
		{
            comps++;
			j++;
		}

	if (j == patternLength) {
		pos = i;
        flag=1; //Set the flag showing that a match has been found
        #pragma omp flush (flag)
    } else {
        comps++;
    }
    }
}
    (*comparisons) = comps;
    return pos;
}

// This method calls the hostMatch method and if -1 is returned then it will report
// no matches, otherwise the actual position of the pattern will be returned along with
// the amount of comparisons ran by all threads.
void processData(int numofthreads)
{
	unsigned int result;
        long comparisons;

	printf ("Text length = %d\n", textLength);
	printf ("Pattern length = %d\n", patternLength);

	result = hostMatch(&comparisons, numofthreads);
	if (result == -1)
		printf ("Pattern not found\n");
	else
		printf ("Pattern found at position %d\n", result);
        printf ("# comparisons = %ld\n", comparisons);

}

//Checks if a test num is specfied it is within the appropriate range
//if not , or one hasnt been specified then algorithm is ran on all test
//cases
int checkTestNum(int testNum){
    if(testNum>=0 && testNum <3){
        return testNum;

    }
    else{
        printf("Running default test cases\n");
        return -1;
    }
}
//Similiar to method above except it checks the num of threads to used as specfied
//by the user. If none is specified then 1 thread is ran
int checkThreads(int threads){
    if(threads>0 && threads<=64){
        printf("Using %d threads \n", threads);
        return threads;
    }
    else{
        printf("Running default number of threads(1)\n");
        threads = 1;
        return threads;
    }
}


int main(int argc, char **argv)
{
    int testNumber;
    struct genargs genargs1;
    uint64_t diff, diff1;
    struct timespec start, end , clock1 , clock2;
    double seconds1, seconds2;
    int Digs = DECIMAL_DIG;

    genargs1.testNum = -1;
    genargs1.numofthreads=-1;
    
    //Reads in the arguments and calls on the arg checker methods
    argp_parse(&argp, argc, argv, 0,0,&genargs1);
    genargs1.testNum = checkTestNum(genargs1.testNum);
    genargs1.numofthreads = checkThreads(genargs1.numofthreads);
    testNumber=0;
    
    // if testNum is set the algorithm will only be ran against the specified test
    if(genargs1.testNum !=-1){
        readData(genargs1.testNum);
	clock_gettime(CLOCK_MONOTONIC, &clock1); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
   	processData(genargs1.numofthreads);
	clock_gettime(CLOCK_MONOTONIC, &clock2); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
        seconds1 = (double)diff / 1000000000.0;
        printf("elapsed time = %.*e Seconds\n", Digs, seconds1);
	diff1 = BILLION * (clock2.tv_sec - clock1.tv_sec) + clock2.tv_nsec - clock1.tv_nsec;
        seconds2 = (double)diff1 / 1000000000.0;
        printf("elapsed process CPU time = %.*e Seconds\n\n", Digs, seconds2);


    }
    else{
    // Will run algorithm against all tests if none are specified both scenarios
    // will return the elapsed time taken
	while (readData (testNumber))
	{
	        clock_gettime(CLOCK_MONOTONIC, &clock1); clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        	processData(genargs1.numofthreads);
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
    return 0;



}
