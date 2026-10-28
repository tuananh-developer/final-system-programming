#ifndef COPY_H
#define COPY_H

/*
 * Copy one regular file.
 * The destination must not already exist.
 * Return 0 on success, -1 on failure (errno is set).
 */
int copy_file(const char *src, const char *dst);

/*
 * Copy a directory recursively.
 * The destination must not already exist.
 * Return 0 on success, -1 on failure (errno is set).
 */
int copy_directory(const char *src, const char *dst);

/*
 * Dispatch a source path to file or directory copy.
 * Symbolic links and special files are not supported.
 */
int copy_path(const char *src, const char *dst);

#endif