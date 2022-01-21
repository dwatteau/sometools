/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2020, 2022 Donovan Watteau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

int
main(int argc, char *argv[])
{
	unsigned char buf[BUFSIZ];
	size_t n, i;
	char *endconvptr;
	long xorkey;

#ifdef _WIN32
	if (_setmode(_fileno(stdin), _O_BINARY) == -1 || _setmode(_fileno(stdout), _O_BINARY) == -1) {
		perror("_setmode()");
		exit(EXIT_FAILURE);
	}
#else
	if (freopen(NULL, "rb", stdin) == NULL || freopen(NULL, "wb", stdout) == NULL) {
		perror("freopen()");
		exit(EXIT_FAILURE);
	}
#endif

	if (argc != 2) {
		fprintf(stderr, "USAGE: %s keyvalue < input > output\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	errno = 0;
	xorkey = strtol(argv[1], &endconvptr, 0);
	if (argv[1][0] == '\0' || *endconvptr != '\0' || errno == ERANGE || errno == EINVAL ||
	    xorkey < 0 || xorkey > UCHAR_MAX) {
		fprintf(stderr, "ERROR: Invalid key; 0-255 or 0x00-0xff value expected\n");
		exit(EXIT_FAILURE);
	}

	while ((n = fread(buf, 1, BUFSIZ, stdin)) != 0) {
		for (i = 0; i < n; i++)
			buf[i] ^= (unsigned char)xorkey;

		if (fwrite(buf, 1, n, stdout) != n) {
			perror("fwrite()");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
