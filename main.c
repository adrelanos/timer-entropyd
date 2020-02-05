#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>

double get_ts(void)
{
        struct timeval ts;
        struct timezone tz;

        if (gettimeofday(&ts, &tz) == -1)
                perror("gettimeofday");

        return (((double)ts.tv_sec) + ((double)ts.tv_usec)/1000000.0);
}

double gen_entropy_data(void)
{
	double start;

	start = get_ts();

	/* arbitrary value:
	 * not too small so that there's room for noise
	 * not too large so that we don't sleep unnecessary
	 */
	usleep(100);

	return get_ts() - start;
}

int main(int argc, char *argv[])
{
	unsigned char bytes[2500];
	unsigned char byte;
	int bits = 0, index = 0;

	printf("timer_entropyd v" VERSION ", (C) 2009-2012 by folkert@vanheusden.com\n\n");

	if (daemon(-1, -1) == -1)
		error_exit("failed to become daemon process");

	if (kernel_rng_open())
		error_exit("failed to open RANDOM device");

	for(;;)
	{
		kernel_rng_poll();

		double t1 = gen_entropy_data(), t2 = gen_entropy_data();

		if (t1 == t2)
			continue;

		byte <<= 1;
		if (t1 > t2)
			byte |= 1;

		if (++bits == 8)
		{
			bytes[index++] = byte;
#ifdef _DEBUG
			printf("%d\r", index);
#endif

			bits = 0;

			if (index == sizeof(bytes))
			{
				int cur_n_bits = determine_number_of_bits_of_data(bytes, sizeof(bytes));

				if(kernel_rng_add_entropy(bytes, sizeof(bytes), cur_n_bits))
					break;

				syslog(LOG_DEBUG, "added %d bits of entropy data", cur_n_bits);
#ifdef _DEBUG
				printf("added %d bits of entropy data\n", cur_n_bits);
#endif

				index = 0;
			}
		}
	}

	kernel_rng_close();

	return 0;
}
