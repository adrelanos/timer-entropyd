#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/random.h>

#include "error.h"
#include "utils.h"

#define DEV_RANDOM	"/dev/random"

int fd;

int kernel_rng_open()
{
	errno = 0;

	fd = open(DEV_RANDOM, O_WRONLY);
	if (fd == -1)
		fprintf(stderr, "Failed to open %s: %d\n", DEV_RANDOM, errno);

	return errno;
}

void kernel_rng_poll()
{
	fd_set wfds;

	FD_ZERO(&wfds);
	FD_SET(fd, &wfds);

	if (select(fd+1, NULL, &wfds, NULL, NULL) == -1)
		error_exit("select() bang");
}

int kernel_rng_add_entropy(unsigned char *data, int n, int n_bits)
{
        struct rand_pool_info *output;

        output = (struct rand_pool_info *)malloc(sizeof(struct rand_pool_info) + n);
        if (!output) {
		fprintf(stderr, "malloc failure in kernel_rng_add_entropy_no_bitcount_increase(%d)\n", n);
		return -1;
	}

	output -> entropy_count = n_bits;
	output -> buf_size      = n;
	memcpy(&(output -> buf[0]), data, n);

	errno = 0;
	if (ioctl(fd, RNDADDENTROPY, output) == -1)
		fprintf(stderr, "ioctl(RNDADDENTROPY) failed: %d\n", errno);

	free(output);

	return errno;
}

void kernel_rng_close()
{
	close(fd);
}
