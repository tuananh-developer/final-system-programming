#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include "copy.h"

#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static char *join_path(const char *parent, const char *name)
{
    size_t parent_len = strlen(parent);
    size_t name_len = strlen(name);

    size_t separator =
        parent_len > 0 && parent[parent_len - 1] != '/' ? 1U : 0U;

    if (parent_len > SIZE_MAX - separator - 1 ||
        name_len > SIZE_MAX - parent_len - separator - 1) {
        errno = ENAMETOOLONG;
        return NULL;
    }

    size_t total_len = parent_len + separator + name_len + 1;
    char *result = malloc(total_len);

    if (result == NULL) {
        return NULL;
    }

    memcpy(result, parent, parent_len);

    size_t pos = parent_len;

    if (separator != 0) {
        result[pos++] = '/';
    }

    memcpy(result + pos, name, name_len);
    result[pos + name_len] = '\0';

    return result;
}

static int validate_destination(const char *src, const char *dst)
{
    char *source_real = realpath(src, NULL);

    if (source_real == NULL) {
        return -1;
    }

    /* Avoid copying the entire filesystem root. */
    if (strcmp(source_real, "/") == 0) {
        free(source_real);
        errno = EINVAL;
        return -1;
    }

    struct stat info;

    if (lstat(dst, &info) == 0) {
        free(source_real);
        errno = EEXIST;
        return -1;
    }

    if (errno != ENOENT) {
        int saved_errno = errno;
        free(source_real);
        errno = saved_errno;
        return -1;
    }

    char *dst_for_dir = strdup(dst);
    char *dst_for_base = strdup(dst);

    if (dst_for_dir == NULL || dst_for_base == NULL) {
        int saved_errno = errno;
        free(dst_for_dir);
        free(dst_for_base);
        free(source_real);
        errno = saved_errno;
        return -1;
    }

    char *parent = dirname(dst_for_dir);
    char *base = basename(dst_for_base);

    char *parent_real = realpath(parent, NULL);

    if (parent_real == NULL) {
        int saved_errno = errno;
        free(dst_for_dir);
        free(dst_for_base);
        free(source_real);
        errno = saved_errno;
        return -1;
    }

    char *destination_real = join_path(parent_real, base);

    free(parent_real);
    free(dst_for_dir);
    free(dst_for_base);

    if (destination_real == NULL) {
        free(source_real);
        return -1;
    }

    size_t source_len = strlen(source_real);

    int is_same = strcmp(source_real, destination_real) == 0;

    int is_inside =
        strncmp(source_real, destination_real, source_len) == 0 &&
        destination_real[source_len] == '/';

    free(source_real);
    free(destination_real);

    if (is_same || is_inside) {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int copy_path(const char *src, const char *dst)
{
    if (src == NULL || src[0] == '\0' ||
        dst == NULL || dst[0] == '\0') {
        errno = EINVAL;
        return -1;
    }

    struct stat info;

    if (lstat(src, &info) < 0) {
        return -1;
    }

    if (S_ISREG(info.st_mode)) {
        return copy_file(src, dst);
    }

    if (S_ISDIR(info.st_mode)) {
        return copy_directory(src, dst);
    }

    errno = ENOTSUP;
    return -1;
}

int copy_directory(const char *src, const char *dst)
{
    if (src == NULL || src[0] == '\0' ||
        dst == NULL || dst[0] == '\0') {
        errno = EINVAL;
        return -1;
    }

    struct stat source_info;

    if (lstat(src, &source_info) < 0) {
        return -1;
    }

    if (!S_ISDIR(source_info.st_mode)) {
        errno = ENOTDIR;
        return -1;
    }

    if (validate_destination(src, dst) < 0) {
        return -1;
    }

    /* Create the destination with temporary owner permissions. */
    if (mkdir(dst, 0700) < 0) {
        return -1;
    }

    DIR *dir = opendir(src);

    if (dir == NULL) {
        int saved_errno = errno;
        (void)rmdir(dst);
        errno = saved_errno;
        return -1;
    }

    int saved_errno = 0;

    while (1) {
        errno = 0;

        struct dirent *entry = readdir(dir);

        if (entry == NULL) {
            if (errno != 0) {
                saved_errno = errno;
            }
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char *src_child = join_path(src, entry->d_name);
        char *dst_child = join_path(dst, entry->d_name);

        if (src_child == NULL || dst_child == NULL) {
            saved_errno = errno;

            free(src_child);
            free(dst_child);
            break;
        }

        if (copy_path(src_child, dst_child) < 0) {
            saved_errno = errno;

            free(src_child);
            free(dst_child);
            break;
        }

        free(src_child);
        free(dst_child);
    }

    if (closedir(dir) < 0 && saved_errno == 0) {
        saved_errno = errno;
    }

    if (saved_errno == 0 &&
        chmod(dst, source_info.st_mode & 0777) < 0) {
        saved_errno = errno;
    }

    if (saved_errno != 0) {
        errno = saved_errno;
        return -1;
    }

    return 0;
}