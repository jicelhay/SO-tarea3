#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
printf("voy a leer el archivo de IP'S\n");
	int c;
	FILE *file;
	file = fopen("ips", "r");
	if (file) {
printf("ENTRE\n");
    	while ((c = getc(file)) != EOF)
       	 putchar(c);
   	fclose(file);
		  }
}



char* substr;
strncpy(substr, buff, 4);


