#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// Read file and return contents in buffer
char *readfile(const char *name)
{
	struct stat status;
	stat(name, &status);
	char *buf = (char *)malloc(status.st_size + 1);
	FILE *file = fopen(name, "r");
	fread(buf, 1, status.st_size, file);
	fclose(file);
	buf[status.st_size] = '\0';
	return buf;
}
