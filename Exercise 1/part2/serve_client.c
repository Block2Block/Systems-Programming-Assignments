#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void* ServeClient(char *client){
	// TODO: Open the client command file and read commands line by line
	FILE *fp = fopen(client, "r");
    char command[50];
    char* splitCommand;
    char* savepntr;

	// TODO: match and execute commands
	while (!feof(fp)) {
	    if (fgets(command, 50, fp) == NULL) {
	        break;
	    } else {
	        splitCommand = strtok_r(command, " ", &savepntr);
	        if (strcmp(splitCommand, "insertNode") == 0) {
	            pthread_mutex_lock(&mutex);
	            char* number = strtok_r(NULL, "\n", &savepntr);
	            printf("[%s]insertNode %s\n", client, number);
	            int value = 0;
	            sscanf(number, "%d", &value);
	            root = insertNode(root, value);
	            pthread_mutex_unlock(&mutex);
	        } else if (strcmp(splitCommand, "deleteNode") == 0) {
                pthread_mutex_lock(&mutex);
                char* number = strtok_r(NULL, "\n", &savepntr);
                printf("[%s]deleteNode %s\n", client, number);
                int value = 0;
                sscanf(number, "%d", &value);
                root = deleteNode(root, value);
                pthread_mutex_unlock(&mutex);
            } else if (strcmp(splitCommand, "sumSubtree\n") == 0) {
                pthread_mutex_lock(&mutex);
                printf("[%s]sumSubtree = %d\n", client, sumSubtree(root));
                pthread_mutex_unlock(&mutex);
            } else if (strcmp(splitCommand, "countNodes\n") == 0) {
                pthread_mutex_lock(&mutex);
                printf("[%s]countNodes = %d\n", client, countNodes(root));
                pthread_mutex_unlock(&mutex);
            }

	    }
	}

	// TODO: Handle command: "insertNode <some unsigned int value>"
	// print: "[ClientName]insertNode <SomeNumber>\n"
	// e.g. "[client1_commands]insertNode 1\n"

	// TODO: Handle command: "deleteNode <some unsigned int value>"
	// print: "[ClientName]deleteNode <SomeNumber>\n"
	// e.g. "[client1_commands]deleteNode 1\n"

	// TODO: Handle command: "countNodes"
	// print: "[ClientName]countNodes = <SomeNumber>\n"
	// e.g. "[client1_commands]countNodes 1\n"


	// TODO: Handle command: "sumSubtree"
	// print: "[ClientName]sumSubtree = <SomeNumber>\n"
	// e.g. "[client1_commands]sumSubtree 1\n"

	fclose(fp);
	return NULL;
}

