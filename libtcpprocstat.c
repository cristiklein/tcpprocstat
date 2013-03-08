#include <dlfcn.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

/**
 * Constants
 */
#define MAX_FDS 1024

/**
 * Variables
 */
static uint32_t _is_socket[MAX_FDS/32]; /*!< fds we are interested in */
static uint32_t bytesSent;
static uint32_t bytesRecv;
static int overflow;

/**
 * Real functions
 */
static ssize_t	(*_read)(int fd, void *buf, size_t count);
static ssize_t	(*_recv)(int s, void *buf, size_t len, int flags);
static ssize_t	(*_write)(int fd, const void *buf, size_t count);
static ssize_t	(*_send)(int s, const void *buf, size_t len, int flags);
static int	(*_socket)(int domain, int type, int protocol);
static int	(*_close)(int fd);

/**
 * Helper functions
 */

/** set this socket for watch */
static void inline
socket_set(int s)
{
	if (s >= MAX_FDS) {
		overflow = 1;
		return;
	}
	_is_socket[s / 32] |= 1 << (s % 32);
}

/** don't watch this socket any more */
static void inline
socket_clear(int s)
{
	if (s >= MAX_FDS) return;
	_is_socket[s / 32] &= ~(1 << (s % 32));
}

/** are we watching this socket */
static int inline
socket_get(int s)
{
	if (s >= MAX_FDS) return 0;
	return _is_socket[s / 32] & (1 << (s % 32));
}

static void inline
_print(char *str)
{
	_write(100, str, strlen(str));
}

/**
 * Function wrappers
 */
ssize_t
recv(int s, void *buf, size_t len, int flags)
{
	int ret = _recv(s, buf, len, flags);
	if (socket_get(s) && \
		(ret > 0) &&
		(flags & MSG_PEEK) == 0) {
			bytesRecv += ret;
	}
	return ret;
}

ssize_t
read(int fd, void *buf, size_t count)
{
	int ret = _read(fd, buf, count);
	if (socket_get(fd) && (ret > 0)) bytesRecv += ret;
	return ret;
}

ssize_t
send(int s, const void *buf, size_t len, int flags)
{
	int ret = _send(s, buf, len, flags);
	if (socket_get(s) && (ret > 0)) bytesSent += ret;
	return ret;
}

ssize_t
write(int fd, const void *buf, size_t count)
{
	int ret = _write(fd, buf, count);
	if (socket_get(fd) && (ret > 0)) bytesSent += ret;
	return ret;
}

int
socket(int domain, int type, int protocol)
{
	int ret = _socket(domain, type, protocol);
	if ((type == SOCK_STREAM) && (ret != -1)) socket_set(ret);
	return ret;
}

int
close(int fd)
{
	socket_clear(fd);
	return _close(fd);
}

#define SYM(x) _##x = dlsym(dl, #x);

/**
 * Entry / Exit point
 */
static void libtcpprocstat_init(void) __attribute__ ((constructor));
static void libtcpprocstat_fini(void) __attribute__ ((destructor));

static void
libtcpprocstat_init(void)
{
	void *dl;

	dl = dlopen("libc.so.6", RTLD_LAZY);
	if (dl == NULL)
	{
		printf("%s\n", dlerror());
		_exit(1);
	}

	SYM(recv);
	SYM(read);
	SYM(send);
	SYM(write);
	SYM(socket);
	SYM(close);

	memset(_is_socket, 0, sizeof(_is_socket));
	bytesSent = 0;
	bytesRecv = 0;

	_print("libtcpprocstat initialized\n");
}
#undef SYM

static void
libtcpprocstat_fini(void)
{
	char buf[100];

	sprintf(buf, "TCP sent: %d\nTCP recv: %d\n", bytesSent, bytesRecv);
	_print(buf);

	if (overflow) 
		_print("WARNING! FD array overflown.\n");
}

