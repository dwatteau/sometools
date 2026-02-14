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

/* TODO: confirm ICC test on Windows and non-Windows */

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_AMD64) || defined(__amd64) || \
    defined(__SSE__) || defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
#	define HAVE_MMX_ALWAYS
#elif defined(__INTEL_COMPILER) && !defined(__GNUC__) && (defined(__i386__) || defined(_M_IX86))
#	define HAVE_INTRIN_CPUID
#elif defined(_WIN32) && (defined(_M_IX86) || defined(_M_AMD64))
#	define HAVE_INTRIN_CPUID
#elif ((defined(__SUNPRO_C) && __SUNPRO_C >= 0x5100) || (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x5100)) && defined(__i386)
#	define HAVE_GCC_INLINE_ASM
#	ifndef __asm__
#		define __asm__ asm
#	endif
#	ifndef __volatile__
#		define __volatile__ volatile
#	endif
#elif defined(__GNUC__) && defined(__i386__)
#	if defined(__clang__) || (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#		define HAVE_GET_CPUID
#	elif __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#		define HAVE_GCC_INLINE_ASM
#	endif
#endif

#ifdef HAVE_GET_CPUID
#include <cpuid.h>
#endif

#ifdef HAVE_INTRIN_CPUID
#include <intrin.h>
#endif

#ifndef bit_MMX
#define bit_MMX		(1 << 23)
#endif

#ifdef HAVE_GCC_INLINE_ASM
/*
 * When calling `cpuid` through inline ASM, check that the CPUID feature
 * itself is available first, as this was introduced with i586.
 */
static int
has_cpuid_support(void)
{
	unsigned int eax, edx;

	__asm__ __volatile__(
		"pushfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"movl %0, %1\n\t"
		"xorl $0x200000, %0\n\t"
		"pushl %0\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %0\n\t"
		"popfl\n\t"
		: "=&r"(eax), "=&r"(edx)
		:
		: "cc"
	);

	return ((eax ^ edx) & 0x200000) != 0;
}
#endif

static int
has_mmx_support(void)
{
#if defined(HAVE_MMX_ALWAYS)
	return 1;
#elif defined(HAVE_GET_CPUID)
	unsigned int eax, ebx, ecx, edx;

	return !!(__get_cpuid(1, &eax, &ebx, &ecx, &edx) && (edx & bit_MMX));
#elif defined(HAVE_INTRIN_CPUID)
	int regs[4];

	__cpuid(regs, 1);
	return !!(regs[3] & bit_MMX);
#elif defined(HAVE_GCC_INLINE_ASM)
	unsigned int eax, ebx, ecx, edx;

	if (!has_cpuid_support())
		return 0;

	/* PIC compatible */
	__asm__ __volatile__(
		"xchgl %%ebx, %1\n\t"
		"cpuid\n\t"
		"xchgl %%ebx, %1\n\t"
		: "=a"(eax), "=&r"(ebx), "=c"(ecx), "=d"(edx)
		: "0"(1)
	);

	return !!(edx & bit_MMX);
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
