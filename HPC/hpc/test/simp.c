#include <stdio.h>
#include <string.h>

int main(void)
{
    const char *pad = "";
    for (int k = 0; k < 20; k++)
    {
        char temp[30]  = "";
        snprintf(temp, sizeof(temp), "val%d", k);
	printf("TEMP : <<%s>>\n" , temp);
    }
    return 0;
}
