#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define ANSI_COLOR_RESET "\033[0m"
#define ANSI_COLOR_GREEN "\033[32m"
#define ANSI_COLOR_CYAN "\033[36m"


void find_file(const char *path, char *name)
{
    int lg = 0;
    if (lg) printf("[LOG] Searching in %s for %s \n", path, name);
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))){
	if (lg) printf("Cannot open directory %s\n", path);
	return;
    };
    if (!(entry = readdir(dir))) {
	if (lg) printf("Cannot read directory %s\n", path);
	return;
    }
    do {
	if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
	if (entry->d_type != DT_DIR) {
	    //Check if search term is a substring of the file name
	    if (strstr(entry->d_name, name) != NULL) {
		printf(ANSI_COLOR_GREEN "Found %s in %s\n" ANSI_COLOR_RESET, name, path);
		printf("\tFull path: " ANSI_COLOR_CYAN "%s/%s\n" ANSI_COLOR_RESET, path, entry->d_name);
	    } 
	    if (lg) printf("[LOG] Not found jpg %s in %s \n", name, path);
	    continue;
	}

	//If it is a directory, recursively call find_file
	//with the new path
	
	//Create new path
	char *path_buf = malloc(strlen(path) + strlen(entry->d_name) + 2);
	strcpy(path_buf, path);
	strcat(path_buf, "/");
	strcat(path_buf, entry->d_name);
	find_file(path_buf, name);
	if (lg) {
	    printf("[LOG] Recursively called find_file with %s \n", path_buf);
	}
	free(path_buf);
    } while ((entry = readdir(dir)));
}

int main(int argc, char *argv[])
{
    //command mini-find <path> <name>

    if (argc != 3) {
	printf("Usage: mini-find <root-path> <query>\n");
	return 1;
    }
    printf("Search started in %s for %s \n", argv[1], argv[2]);
    find_file(argv[1], argv[2]);
    return 0;
}
