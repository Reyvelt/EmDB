#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
}

int main(int argc, char *argv[]) { 
	int c = 0;
	char *filePath = NULL;
	char *addstring = NULL;
	char *removestring = NULL;
	bool newFile = false;
	bool list = false;

	int dataBaseFileDescriptor = -1;	
	struct dbheader_t *dataBaseHeader = NULL;
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "f:lna:r:")) != -1) {
		switch(c) {
			case 'f':
				filePath = optarg;
				break;
			case 'l':
				list = true;
				break;
			case 'n':
				newFile = true;
				break;
			case 'a':
				addstring = optarg;
				break;
			case 'r':
				removestring = optarg;
				break;
			case '?':
				printf("Unknown option -%c\n", c);
				break;
			default:
				return STATUS_ERROR;
		}
	}

	if (filePath == NULL) {
		printf("Filepath is a required argument\n");
		print_usage(argv);

		return STATUS_SUCCESS;
	}

	if (newFile) {
		dataBaseFileDescriptor = create_db_file(filePath);
		if (dataBaseFileDescriptor == STATUS_ERROR) {
			printf("Unable to create database file.\n");
			return STATUS_ERROR;
		}
		if (create_db_header(dataBaseFileDescriptor, &dataBaseHeader) == STATUS_ERROR){
			printf("Failed to create database header\n");
			return STATUS_ERROR;
		}
	} else {
		dataBaseFileDescriptor = open_db_file(filePath);
		if (dataBaseFileDescriptor == STATUS_ERROR) {
			printf("Unable to open database file.\n");
			return STATUS_ERROR;
		}
		if (validate_db_header(dataBaseFileDescriptor, &dataBaseHeader) == STATUS_ERROR) {
			printf("Failed to validate database header\n");
			return STATUS_ERROR;
		}
	}

	if(read_employees(dataBaseFileDescriptor, dataBaseHeader, &employees) != STATUS_SUCCESS) {
		printf("Failed to read employees");
		return 0;
	}

	if(addstring) {
		dataBaseHeader->count++;
		employees = realloc(employees, dataBaseHeader->count*(sizeof(struct employee_t)));
		add_employee(dataBaseHeader, employees, addstring);
	}

	if(list) {
		list_employees(dataBaseHeader, employees);
	} 

	if(removestring) {
		int removeIndex = find_index(dataBaseHeader, employees, removestring);
		if (removeIndex != STATUS_ERROR) {
			remove_employee(dataBaseHeader, employees, removeIndex);
		}
		dataBaseHeader->count--;
		employees = realloc(employees, dataBaseHeader->count*(sizeof(struct employee_t)));
		list_employees(dataBaseHeader, employees);
	}

	output_file(dataBaseFileDescriptor, dataBaseHeader, employees);

	return 0;
}
