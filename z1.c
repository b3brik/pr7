#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH 4096
#define MAX_LINE 8192

void search_in_file(const char *filepath, const char *word) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror(filepath);
        return;
    }

    char line[MAX_LINE];
    int line_number = 0;
    int found_any = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        char *found = strstr(line, word);
        if (found) {
            printf("%s:%d: %s\n", filepath, line_number, word);
            found_any = 1;
        }
    }

    if (!found_any) {
        printf("%s: слово \"%s\" не найдено\n", filepath, word);
    }

    fclose(file);
}

void search_directory(const char *dirpath, const char *word) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror(dirpath);
        return;
    }

    struct dirent *entry;
    char path[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);

        struct stat statbuf;
        if (lstat(path, &statbuf) == -1) {
            perror(path);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            search_directory(path, word);
        } else if (S_ISREG(statbuf.st_mode)) {
            search_in_file(path, word);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *dirpath = getenv("HOME");
    static char default_path[MAX_PATH];
    const char *word = NULL;

    if (argc < 2) {
        fprintf(stderr, "Использование: %s <word> [directory]\n", argv[0]);
        return EXIT_FAILURE;
    }

    word = argv[1];
    if (argc >= 3) {
        dirpath = argv[2];
    } else {
        snprintf(default_path, sizeof(default_path), "%s/files", dirpath);
        dirpath = default_path;
    }

    search_directory(dirpath, word);
    return EXIT_SUCCESS;
}