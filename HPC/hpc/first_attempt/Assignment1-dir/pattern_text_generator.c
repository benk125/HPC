/**
 * C program to check whether a directory exists or not.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int isDirectoryExists(const char *path);
int makeDirectory(const char *path, int e);
int makeFile(const char *pattern, const char *elem1 , const char *elem2, int pattern_length, int text_length);

int main( int argc, char *argv[])
{
    int e;
    char path[20] = "./data";
    char pattern[30] = "";
    const char *elem1 = argv[1];
    const char *elem2 = argv[2];
    int pattern_length = atoi(argv[3]);
    int text_length = atoi(argv[4]);
    int test_file_num = atoi(argv[5]);

    makeDirectory(path , e);
    
    sprintf(path, "./data/test%d", test_file_num);
    makeDirectory(path, e);
    sprintf(pattern, "./data/test%d/pattern.txt", test_file_num);
    makeFile(pattern , elem1 , elem2 , pattern_length, text_length);
    sprintf(pattern, "./data/test%d/text.txt", test_file_num);
    makeFile(pattern, elem1, elem2, pattern_length, text_length);
    return 0;
}

int makeFile(const char *pattern, const char *elem1 , const char *elem2, int pattern_length, int text_length){

        if (isDirectoryExists(pattern))
        {
                printf("File exists at path '%s'\n", pattern);
        }
	else{
		printf("pattern is '%s'\n", pattern);
		if(strstr(pattern, "pattern") != NULL){
		    FILE *out = fopen(pattern, "a");
		    for(int i=0; i< (pattern_length - 1); i++){
  			  fprintf(out, "%s", elem1);  
		    }
		    fprintf(out, "%s", elem2);
  		    fclose(out);
		}
		if(strstr(pattern, "text") != NULL){
			FILE *file_ptr = fopen(pattern, "a");
			for(int i=0; i< (text_length - 1); i++){
                          fprintf(file_ptr, "%s", elem1);
                    }
                    fprintf(file_ptr, "%s", elem2);
                    fclose(file_ptr);
		}
	}
} 


int makeDirectory(const char *path , int e){
        if (isDirectoryExists(path))
        {
                printf("Directory exists at path '%s'\n", path);
        }
        else
        {
                printf("Directory does not exists at path '%s'\n .. Creating Directory \n" , path);
                e = mkdir(path, S_IRWXU);
                if (e != 0){
                        printf("mkdir failed; error : %d \n", e);
                }
                else{
                        printf("created the directory %s\n",path);
                }
        }
}



/**
 * Function to check whether a directory exists or not.
 * It returns 1 if given path is directory and  exists 
 * otherwise returns 0.
 */
int isDirectoryExists(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}
