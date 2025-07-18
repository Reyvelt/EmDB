#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

int find_index(struct dbheader_t *dbhdr, struct employee_t *employees, char *findstring) {
	int count = dbhdr->count;
	int i = 0;
	for (; i < count; i++) {
		char *name = employees[i].name;
		if(*name == *findstring){
			return i;
		}
	}
	printf("Could not find employee\n");
	return STATUS_ERROR;
}

int remove_employee(struct dbheader_t *dbhdr, struct employee_t *employees, int removeIndex) {
	int count = dbhdr->count;
	int i = removeIndex;

	for (; i < count-1; i++) {
		employees[i] = employees[i + 1];
	}
	return STATUS_SUCCESS;
	
}

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
	int i = 0;
	for (; i < dbhdr->count; i++) {
		printf("Employee %d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tLogged Hours: %d\n", employees[i].hours);
	}
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

	char *name = strtok(addstring, ",");
	char *addr = strtok(NULL, ",");
	char *hours = strtok(NULL, ",");

	printf("New Employee Added:\n");

	printf("\tName: %s\n", name);
	printf("\tAddress: %s\n", addr);
	printf("\tLogged Hours: %s\n", hours);

	strncpy(employees[dbhdr->count-1].name, name, sizeof(employees[dbhdr->count-1].name));
	strncpy(employees[dbhdr->count-1].address, addr, sizeof(employees[dbhdr->count-1].address));
	employees[dbhdr->count-1].hours = atoi(hours);
		
	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	if (fd < 0) {
		printf("Got a bad file descriptor from user\n");
		return STATUS_ERROR;
	}

	int count = dbhdr->count;

	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if (employees == -1) {
		printf("Memory allocation failed\n");
		return STATUS_ERROR;
	}

	read(fd, employees, count*sizeof(struct employee_t));

	int i = 0;
	for(; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	}

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *header, struct employee_t *employees) {
	if (fd < 0) {
		printf("Got a bad file descriptor from user\n");
		return STATUS_ERROR;
	}
	int realCount = header->count;
	int realFileSize = sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realCount);
	
	header->magic = htonl(header->magic);
	header->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realCount));
	header->count = htons(header->count);
	header->version = htons(header->version);
		
	lseek(fd, 0, SEEK_SET);

	write(fd, header, sizeof(struct dbheader_t));

	int i = 0;
	for (; i < realCount; i++) {
		employees[i].hours = htonl(employees[i].hours);
		write(fd, &employees[i], sizeof(struct employee_t));
	}

	if (ftruncate(fd, realFileSize) == -1) {
		perror("ftruncate");
		return STATUS_ERROR;
	}

	return STATUS_SUCCESS;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("Got a bad file descriptor from user\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if(header == -1){
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->magic != HEADER_MAGIC) {
		printf("Improper header magic\n");
		free(header);
		return STATUS_ERROR;
	}

	if (header->version != 1) {
		printf("Improper header version\n");
		free(header);
		return STATUS_ERROR;
	}

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Corrupted database\n");
		free(header);
		return STATUS_ERROR;
	}

	*headerOut = header;

	return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if(header == -1){
		printf("Malloc failed to create db header\n");
		return STATUS_ERROR;
	}
	header->version = 0x1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);
	
	*headerOut = header;

	return STATUS_SUCCESS;
}


