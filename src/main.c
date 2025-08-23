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
	int employeeIndex = -1;
	char *filePath = NULL;
	char *addString = NULL;
	char *searchName = NULL;
  char *updateString = NULL;
	bool removeEmployee = false;
	bool newFile = false;
	bool listEmployee = false;

	int dataBaseFileDescriptor = -1;	
	struct dbheader_t *dataBaseHeader = NULL;
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "f:lna:ds:u:")) != -1) {
		switch(c) {
			case 'f':
				filePath = optarg;
				break;
			case 'l':
				listEmployee = true;
				break;
			case 'n':
				newFile = true;
				break;
			case 'a':
				addString = optarg;
				break;
			case 'd':
				removeEmployee = true;
				break;
			case 's':
				searchName = optarg;
				break;
      case 'u':
        updateString = optarg;
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
		if (create_db_header(&dataBaseHeader) == STATUS_ERROR){
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

	if(addString) {
    
		add_employee(dataBaseHeader, &employees, addString);
	}

	if(searchName) {
		employeeIndex = find_index(dataBaseHeader, employees, searchName, 0);
	}

	if(listEmployee) {
		list_employees(dataBaseHeader, employees);
	} 

  if(updateString) {
		if (employeeIndex == -1) {
			printf("No employee selected");
			return 0;
		}
    update_employee(employees, updateString, employeeIndex);
  }

	if(removeEmployee) {
		if (employeeIndex == -1) {
			printf("No employee selected");
			return 0;
		}
		remove_employee(dataBaseHeader, employees, employeeIndex);
		dataBaseHeader->count--;
		employees = realloc(employees, dataBaseHeader->count*(sizeof(struct employee_t)));
	}

	output_file(dataBaseFileDescriptor, dataBaseHeader, employees);

	return 0;
}
