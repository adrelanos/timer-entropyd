#include <string.h>
#include <math.h>

#include "utils.h"

int determine_number_of_bits_of_data(unsigned char *data, int n_bytes)
{
	int cnts[256], loop;
	double ent=0.0;

	memset(cnts, 0x00, sizeof(cnts));

	for(loop=0; loop<n_bytes; loop++)
	{
		cnts[data[loop]]++;
	}

	for(loop=0; loop<256;loop++)
	{
		double prob = (double)cnts[loop] / (double)n_bytes;

		if (prob > 0.0)
		{
			ent += prob * (log(1.0/prob)/log(2.0));
		}
	}

	ent *= (double)n_bytes;

	if (ent < 0.0) ent=0.0;

	ent = min((double)(n_bytes*8), ent);

	return ent;
}
