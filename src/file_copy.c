#define _POSIX_C_SOURCE 200809L

#include "copy.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define COPY_BUF_SIZE 65536

static int write_full(int fd, const unsigned char *buf, size_t len)
{
    size_t off = 0;

    while (off < len) {
        ssize_t n = write(fd, buf + off, len - off);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            errno = EIO;
            return -1;
        }
        off += (size_t)n;
    }

    return 0;
}

int copy_file(const char *src, const char *dst)
{
    if (src == NULL || dst == NULL) {
        errno = EINVAL;
        return -1;
    }

    struct stat st;
    if (lstat(src, &st) != 0) {
        return -1;
    }

    if (!S_ISREG(st.st_mode)) {
        errno = ENOTSUP;
        return -1;
    }

    struct stat dst_st;
    if (lstat(dst, &dst_st) == 0) {
        errno = EEXIST;
        return -1;
    }
    if (errno != ENOENT) {
        return -1;
    }

    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        return -1;
    }

    mode_t mode = st.st_mode & 0777;
    int dst_fd = open(dst, O_WRONLY | O_CREAT | O_EXCL, mode);
    if (dst_fd < 0) {
        int saved_errno = errno;
        close(src_fd);
        errno = saved_errno;
        return -1;
    }

    unsigned char *buf = (unsigned char *)malloc(COPY_BUF_SIZE);
    if (buf == NULL) {
        int saved_errno = errno;
        close(dst_fd);
        close(src_fd);
        errno = saved_errno;
        return -1;
    }

    int rc = 0;
    for (;;) {
        ssize_t r = read(src_fd, buf, COPY_BUF_SIZE);
        if (r < 0) {
            if (errno == EINTR) {
                continue;
            }
            rc = -1;
            break;
        }
        if (r == 0) {
            break;
        }
        if (write_full(dst_fd, buf, (size_t)r) != 0) {
            rc = -1;
            break;
        }
    }

    int saved_errno = errno;
    free(buf);

    if (close(dst_fd) != 0 && rc == 0) {
        saved_errno = errno;
        rc = -1;
    }
    if (close(src_fd) != 0 && rc == 0) {
        saved_errno = errno;
        rc = -1;
    }

    if (rc != 0) {
        errno = saved_errno;
        return -1;
    }

    return 0;
}
