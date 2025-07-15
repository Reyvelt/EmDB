#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"


int create_db_file(char *filename) {
	int fileDescriptor = open(filename, O_RDONLY);
	if (fileDescriptor != -1) {
		close(fileDescriptor);
		printf("NewFileError: Database already exists.\n");
		return STATUS_ERROR;
	}

	fileDescriptor = open(filename, O_RDWR | O_CREAT, 0644);
	if (fileDescriptor == -1) {
		perror("open");
		return STATUS_ERROR;
	}

	return fileDescriptor;
}

int open_db_file(char *filename) {
	int fileDescriptor = open(filename, O_RDWR, 0644);
	if (fileDescriptor == -1){
		perror("open");
		return STATUS_ERROR;
	}

	return fileDescriptor;
}


