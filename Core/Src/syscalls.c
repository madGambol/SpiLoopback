/**
 * Minimal syscall stubs for newlib on bare-metal ARM.
 * These stubs silence linker warnings about unimplemented syscalls.
 */

#include <sys/stat.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

int _close(int fd)
{
    (void)fd;
    errno = EBADF;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    (void)st;
    errno = EBADF;
    return -1;
}

int _getpid(void)
{
    return 1;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _lseek(int fd, int offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    errno = EBADF;
    return -1;
}

int _read(int fd, char *buf, int len)
{
    (void)fd;
    (void)buf;
    (void)len;
    errno = EBADF;
    return -1;
}

int _write(int fd, char *buf, int len)
{
    (void)fd;
    (void)buf;
    (void)len;
    errno = EBADF;
    return -1;
}

#ifdef __cplusplus
}
#endif
