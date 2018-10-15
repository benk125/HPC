#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

extern int errno;

int main(){
	int e;
	struct stat sb;
	char *name = "test";

	e = stat(name, &sb);
	printf("e=%d errno=%d\n",e,errno);
	if (e == 0){
		if (sb.st_mode & S_IFDIR)
			printf("%s is a directory.\n",name);
		if (sb.st_mode & S_IFREG)
			printf("%s is a regular file.\n",name);
	}
	else{
		if (errno = ENOENT){
			printf("The directory does not exist. Creating new directory...\n");
			e = mkdir(name, S_IRWXU);
			if (e != 0){
				printf("mkdir failed; errno=%d\n",errno);
			}
			else{
				printf("created the directory %s\n",name);
			    }
		}
	}
	return 0;
}

