#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_PATH 4096

char *strcasestr_local(const char *haystack, const char *needle) {
    size_t needle_len = strlen(needle);
    if (needle_len == 0) return (char *)haystack;

    for (; *haystack; ++haystack) {
        if (strncasecmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
    }
    return NULL;
}

void print_line_with_context(const char *start, const char *file_start, const char *file_end,
                             const char *filepath, int line_num, const char *word) {
    printf("%s:%d: %s\n", filepath, line_num, word);
}

void search_in_file_mmap(const char *filepath, const char *word, int ignore_case) {
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        perror(filepath);
        return;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return;
    }

    if (st.st_size == 0) {
        close(fd);
        return;
    }

    char *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return;
    }

    const char *ptr = data;
    const char *end = data + st.st_size;
    int line_number = 1;
    int found_any = 0;

    while (ptr < end) {
        const char *match = NULL;

        if (ignore_case) {
            match = strcasestr_local(ptr, word);
        } else {
            match = strstr(ptr, word);
        }

        if (!match || match >= end)
            break;

        for (const char *c = ptr; c < match; c++) {
            if (*c == '\n') line_number++;
        }

        print_line_with_context(match, data, end, filepath, line_number, word);
        found_any = 1;
        ptr = match + 1;
    }

    if (!found_any) {
        printf("%s: слово \"%s\" не найдено\n", filepath, word);
    }

    munmap(data, st.st_size);
    close(fd);
}

void search_directory(const char *dirpath, const char *word, int ignore_case) {
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
            search_directory(path, word, ignore_case);
        } else if (S_ISREG(statbuf.st_mode)) {
            search_in_file_mmap(path, word, ignore_case);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *word = NULL;
    const char *dirpath = NULL;
    static char default_path[MAX_PATH];
    int ignore_case = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            ignore_case = 1;
        } else if (!word) {
            word = argv[i];
        } else if (!dirpath) {
            dirpath = argv[i];
        }
    }

    if (!word) {
        fprintf(stderr, "Использование: %s <word> [-i] [directory]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!dirpath) {
        const char *home = getenv("HOME");
        if (!home) {
            fprintf(stderr, "Что-то не так с директорией хоум.\n");
            return EXIT_FAILURE;
        }
        snprintf(default_path, sizeof(default_path), "%s/files", home);
        dirpath = default_path;
    }

    search_directory(dirpath, word, ignore_case);
    return EXIT_SUCCESS;
}