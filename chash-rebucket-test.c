/*
chash-rebucket-test: shows that chash is consistent
Copyright (C) 2018 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

	https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chash.h>

#define THRESHOLD 0.01

#define atosize_t(x) ((size_t)strtoul(x, NULL, 10))

int main(int argc, char **argv)
{
    size_t i, stayed, moved;
    int verbose, fail;
    size_t key, first_key, num_keys;
    double ratio, ideal, diff;
    size_t lens[5];
    struct chash_t *chash4, *chash5;
    char key_buf[80];
    const char *first, *next;
    size_t flen, nlen;
    const char *node_names[] =
	{ "server1", "server2", "server3", "server4", "server5" };

    for (i = 0; i < 5; ++i) {
	lens[i] = strlen(node_names[i]);
    }

    chash4 = chash_create(node_names, lens, 4, 160);
    chash5 = chash_create(node_names, lens, 5, 160);

    verbose = argc > 1 ? atoi(argv[1]) : 0;
    num_keys = argc > 2 ? atosize_t(argv[2]) : 10 * 1000 * 1000;
    first_key = argc > 3 ? atosize_t(argv[3]) : 0;

    ideal = 4.0 / 5.0;
    stayed = 0;
    moved = 0;
    for (key = first_key; key < (first_key + num_keys); ++key) {
	sprintf(key_buf, "%lu", (unsigned long) key);
	chash_lookup(chash4, key_buf, strlen(key_buf), &first, &flen);
	chash_lookup(chash5, key_buf, strlen(key_buf), &next, &nlen);

	if (strcmp(first, next) == 0) {
	    ++stayed;
	} else {
	    ++moved;
	}
    }
    ratio = ((double) stayed) / ((double) (moved + stayed));
    diff = ratio >= ideal ? ratio - ideal : ideal - ratio;
    fail = !(diff < THRESHOLD);

    if (verbose || fail) {
	printf("tested %llu keys (%llu <= %llu)\n",
	       (unsigned long long) num_keys,
	       (unsigned long long) first_key,
	       (unsigned long long) (first_key + num_keys));
	printf("with 4 buckets and 5 buckets\n");
	printf("%llu stayed in the same bucket, %llu moved buckets\n",
	       (unsigned long long) stayed, (unsigned long long) moved);
	printf("for a ratio of %g, ideal would be %g, diff: %g\n",
	       ratio, ideal, diff);
    }

    if (fail) {
	fprintf(stderr, "FAIL: diff == %g, expected < %g\n", diff, THRESHOLD);
	return 1;
    } else if (verbose) {
	printf("PASS\n");
    }

    return 0;
}
