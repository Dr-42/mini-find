#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>

#define ANSI_COLOR_RESET "\033[0m"
#define ANSI_COLOR_RED "\033[31m"
#define ANSI_COLOR_GREEN "\033[32m"
#define ANSI_COLOR_CYAN "\033[36m"
#define ANSI_COLOR_BLUE "\033[34m"
#define ANSI_COLOR_YELLOW "\033[33m"

typedef enum print_level_t {
    LOG = 0,
    ERROR = 1
} print_level_t;

int lg = false;
int elg = true;

void mlog(print_level_t level, char *message, ...) {
    va_list args;

    va_start(args, message);
    switch (level) {
	case LOG: {
	    if (!lg) return;
	    printf("%sLOG:%s ", ANSI_COLOR_BLUE, ANSI_COLOR_RESET);
	} break;
	case ERROR: {
	    if (!elg) return;
	    printf("%sERROR:%s ", ANSI_COLOR_RED, ANSI_COLOR_RESET);
	} break;
    }
    vprintf(message, args);
    printf("\n");
    va_end(args);
}

void find_file(const char *path, char *name) {
    mlog(LOG, "Searching in %s for %s", path, name);
    DIR *dir;
    struct dirent *entry;

    errno = 0;
    dir = opendir(path);
    if (dir == NULL && elg) {
	if (errno != 0) {
	    mlog(ERROR, "Error reading directory %s: %s", path, strerror(errno));
	}
	return;
    }

    errno = 0;
    while ((entry = readdir(dir)) != NULL) {
	if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
	    continue;
	if (entry->d_name[0] == '.') {
	    continue;
	}

	if (entry->d_type == DT_DIR) {
	    char *path_buf = malloc(strlen(path) + strlen(entry->d_name) + 2);
	    if (path_buf == NULL) {
		//if (elg) printf("%sERROR:%s malloc failed: %s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET, strerror(errno));
		mlog(ERROR, "malloc failed: %s", strerror(errno));
		free(path_buf);
	    } else {
		sprintf(path_buf, "%s/%s", path, entry->d_name);
		find_file(path_buf, name);
		//if (lg) printf("[LOG] Recursively called find_file with %s\n", path_buf);
		mlog(LOG, "Recursively called find_file with %s", path_buf);
		free(path_buf);
	    }
	} else {
	    if (strcmp(entry->d_name, name) == 0) {
		printf(ANSI_COLOR_GREEN "File with name %s in %s\n" ANSI_COLOR_RESET, name, path);
		printf("\tFull path: " ANSI_COLOR_YELLOW "%s/%s\n" ANSI_COLOR_RESET, path, entry->d_name);
	    } else if (strstr(entry->d_name, name) != NULL) {
		printf(ANSI_COLOR_BLUE "Partial match found in %s in %s\n" ANSI_COLOR_RESET, name, path);
		printf("\tFull path: " ANSI_COLOR_CYAN "%s/%s\n" ANSI_COLOR_RESET, path, entry->d_name);
	    } else {
		//if (lg) printf("[LOG] Not found %s in %s\n", name, path);
		mlog(LOG, "Not found %s in %s", name, path);
	    }
	}
    }

    if (errno != 0) {
	//if (elg) printf("%sERROR:%s Error reading directory %s: %s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET, path, strerror(errno));
	mlog(ERROR, "Error reading directory %s: %s", path, strerror(errno));
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
	printf("Usage: mini-find <root-path> <query>\n");
	return 1;
    }
    printf("Search started in %s for %s\n", argv[1], argv[2]);
    find_file(argv[1], argv[2]);
    return 0;
}
