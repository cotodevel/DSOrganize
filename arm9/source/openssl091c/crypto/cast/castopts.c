/* crypto/cast/castopts.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

/* define PART1, PART2, PART3 or PART4 to build only with a few of the options.
 * This is for machines with 64k code segment size restrictions. */

#ifndef MSDOS
#define TIMES
#endif

#include <stdio.h>
#ifndef MSDOS
#include <unistd.h>
#else
#include <io.h>
extern void exit();
#endif
#include <signal.h>
#ifndef VMS
#ifndef _IRIX
#include <time.h>
#endif
#ifdef TIMES
#include <sys/types.h>
#include <sys/times.h>
#endif
#else /* VMS */
#include <types.h>
struct tms {
	time_t tms_utime;
	time_t tms_stime;
	time_t tms_uchild;	/* I dunno...  */
	time_t tms_uchildsys;	/* so these names are a guess :-) */
	}
#endif
#ifndef TIMES
#include <sys/timeb.h>
#endif

#if defined(sun) || defined(__ultrix)
#define _POSIX_SOURCE
#include <limits.h>
#include <sys/param.h>
#endif

#include "cast.h"

#define CAST_DEFAULT_OPTIONS

#undef E_CAST
#define CAST_encrypt  CAST_encrypt_normal
#define CAST_decrypt  CAST_decrypt_normal
#define CAST_cbc_encrypt  CAST_cbc_encrypt_normal
#undef HEADER_CAST_LOCL_H
#include "c_enc.c"

#define CAST_PTR
#undef CAST_PTR2
#undef E_CAST
#undef CAST_encrypt
#undef CAST_decrypt
#undef CAST_cbc_encrypt
#define CAST_encrypt  CAST_encrypt_ptr
#define CAST_decrypt  CAST_decrypt_ptr
#define CAST_cbc_encrypt  CAST_cbc_encrypt_ptr
#undef HEADER_CAST_LOCL_H
#include "c_enc.c"

#undef CAST_PTR
#define CAST_PTR2
#undef E_CAST
#undef CAST_encrypt
#undef CAST_decrypt
#undef CAST_cbc_encrypt
#define CAST_encrypt  CAST_encrypt_ptr2
#define CAST_decrypt  CAST_decrypt_ptr2
#define CAST_cbc_encrypt  CAST_cbc_encrypt_ptr2
#undef HEADER_CAST_LOCL_H
#include "c_enc.c"

/* The following if from times(3) man page.  It may need to be changed */
#ifndef HZ
# ifndef CLK_TCK
#  ifndef _BSD_CLK_TCK_ /* FreeBSD fix */
#   ifndef VMS
#    define HZ	100.0
#   else /* VMS */
#    define HZ	100.0
#   endif
#  else /* _BSD_CLK_TCK_ */
#   define HZ ((double)_BSD_CLK_TCK_)
#  endif
# else /* CLK_TCK */
#  define HZ ((double)CLK_TCK)
# endif
#endif

#define BUFSIZE	((long)1024)

#ifndef NOPROTO
double Time_F(int s);
#else
double Time_F();
#endif

#if defined(__STDC__) || defined(sgi)
#define SIGRETTYPE void
#else
#define SIGRETTYPE int
#endif

#ifndef NOPROTO
extern SIGRETTYPE sig_done(int sig);
#else
extern SIGRETTYPE sig_done();
#endif


#define START	0
#define STOP	1

#ifdef SIGALRM
#define print_name(name) fprintf(stderr,"Doing %s's for 10 seconds\n",name); alarm(10);
#else
#define print_name(name) fprintf(stderr,"Doing %s %ld times\n",name,cb);
#endif
	

#define print_it(name,index) \
	fprintf(stderr,"%s bytes per sec = %12.2f (%5.1fuS)\n",name, \
		tm[index]*8,1.0e6/tm[index]);

