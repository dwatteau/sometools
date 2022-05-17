/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2022 Donovan Watteau
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

#include <stdio.h>

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_AMD64)
#  define HAVE_MMX_ALWAYS
#elif defined(__i386__) && (defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))))
#  define HAVE_GET_CPUID
#elif defined(_WIN32) && (defined(_M_IX86) || defined(_M_AMD64))
#  define HAVE_MS_CPUID
#endif

#ifdef HAVE_GET_CPUID
#include <cpuid.h>
#endif

#ifndef bit_MMX
#define bit_MMX		(1 << 23)
#endif

static int
has_mmx_support(void)
{
#if defined(HAVE_MMX_ALWAYS)
	return 1;
#elif defined(HAVE_GET_CPUID)
	unsigned int eax, ebx, ecx, edx;

	return !!(__get_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & bit_MMX));
#elif defined(HAVE_MS_CPUID)
	int regs[4] = { -1 };

	__cpuid(regs, 1);
	return !!(regs[3] & bit_MMX);
#else
	return -1;
#endif
}

int
main(void)
{
	int res;

	res = has_mmx_support();
	if (res == -1)
		puts("MMX support: unknown");
	else if (res)
		puts("MMX support: yes");
	else
		puts("MMX support: no");

	return 0;
}
