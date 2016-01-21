/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Wang Jian
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <sys/stat.h>

#include "st_macro.h"
#include "st_log.h"
#include "st_utils.h"

static long mask = ((((long)1) << 32) - 1);

long make_long(unsigned int a, unsigned int b)
{
    return (((long)a) << 32) | ((long)b);
}

void split_long(long l, unsigned int *a, unsigned int *b)
{
    ST_CHECK_PARAM_VOID(a == NULL || b == NULL)

    *a = (unsigned int)(l >> 32);
    *b = (unsigned int)(l & mask);
}

void remove_newline(char *line)
{
    char *pstr = NULL;

    ST_CHECK_PARAM_VOID(line == NULL);

    pstr = strrchr(line, '\r');
    if(pstr != NULL)
    {
        *pstr = 0;
    }
    pstr = strrchr(line, '\n');
    if(pstr != NULL)
    {
        *pstr = 0;
    }
}
 
void remove_leading_space(char *line)
{
    char *pstr = NULL;
    char *p = NULL;

    ST_CHECK_PARAM_VOID(line == NULL);

    pstr = line;
    while(*pstr != '\0' && (*pstr == ' ' || *pstr == '\t')) {
        ++pstr;
    }

    if (pstr != line) {
        p = line;
        while(*pstr != '\0') {
            *p = *pstr;
            p++;
            pstr++;
        } 
        *p = '\0';
    }
}
 
void trim(char *line)
{
    char *pstr = NULL;

    ST_CHECK_PARAM_VOID(line == NULL);

    remove_newline(line);
    remove_leading_space(line);

    pstr = line + strlen(line) - 1;
    while(pstr >= line && (*pstr == ' ' || *pstr == '\t')) {
        --pstr;
    }

    pstr++;
    if (pstr >= line) {
        *pstr = '\0';
    }
}
 
int split_line(const char *line, char *fields, 
        int n_field, int field_len, const char *seps)
{
    const char *p;
    const char *q;
    int f;
    int i;
    bool split;

    ST_CHECK_PARAM(fields == NULL || line == NULL, -1);

    p = line;
    f = 0;
    i = 0;
    while (*p != '\0') {
        split = false;
        while (*p != '\0') {
            q = seps;
            while (*q != '\0') {
                if (*p == *q) {
                    split = true;
                    break;
                }
                q++;
            }
            if (*q == '\0') { /* not meet seps */
                break;
            }
            p++;
        }

        if (*p == '\0') {
            break;
        }

        if (split && i > 0) {
            fields[f * field_len + i] = '\0';
            if (f >= n_field - 1) {
                ST_WARNING("Too many fields. [%s]", line);
                return -1;
            }
            f++;
            i = 0;
        }

        if (i >= field_len - 1) {
            ST_WARNING("Too long field. [%s]", p - i);
            return -1;
        }
        fields[f * field_len + i] = *p;
        i++;

        p++;
    }

    if (i > 0) {
        fields[f * field_len + i] = '\0';
        f++;
    }

    return f;
}

unsigned int highest_bit_mask(unsigned int num, int overflow)
{
    unsigned int mask;
    if(overflow)
    {
        mask = num;
    }
    else
    {
        mask = num >> 1;
    }

    mask = mask | (mask >> 1);
    mask = mask | (mask >> 2);
    mask = mask | (mask >> 4);
    mask = mask | (mask >> 8);
    mask = mask | (mask >> 16);

    return mask;
}

void iqsort(int *arr, int l, int u,
        int cmp(int *arr, int i, int j, void *args), void *args)
{
    int i;
    int j;
    int t;

    ST_CHECK_PARAM_VOID(arr == NULL || cmp == NULL || l < 0);

    if(l >= u)
    {
        return;
    }

    i = l;
    j = u + 1;
    while(1)
    {
        do{
            ++i;
        } while(i <= u && cmp(arr, i , l, args) < 0); // arr[i] <= t

        do{
            --j;
        } while(cmp(arr, j, l, args) > 0);

        if(i > j)
        {
            break;
        }

        t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
    t = arr[l];
    arr[l] = arr[j];
    arr[j] = t;

    iqsort(arr, l, j-1, cmp, args); 
    iqsort(arr, j+1, u, cmp, args); 
}

const char* get_next_token(const char *line, char *token)
{
    while((*line == ' ' || *line == '\t') && *line)
    {
        line++;
    }

    while(*line && *line != ' ' && *line != '\t')
    {
        *token = *line;
        token++;
        line++;
    }

    *token = 0;
    if(*line == 0)
    {
        line = NULL;
    }

    return line;
}

static int get_next_utf8_char(const char *utf8)
{
    unsigned char lb;

    ST_CHECK_PARAM(utf8 == NULL, -1);

    lb = *utf8;
    if(lb == 0)
    {
        return -1;
    }

    if (( lb & 0x80 ) == 0 )          // lead bit is zero, must be a single ascii
    {
        return 1;
    }
    else if (( lb & 0xE0 ) == 0xC0 )  // 110x xxxx
    {
        return 2;
    }
    else if (( lb & 0xF0 ) == 0xE0 ) // 1110 xxxx
    {
        return 3;
    }
    else if (( lb & 0xF8 ) == 0xF0 ) // 1111 0xxx
    {
        return 4;
    }
    else
    {
        ST_WARNING( "Unrecognized UTF8 lead byte (%02x)\n", lb );
        return -1;
    }
}

static int get_next_gbk_char(const char *gbk)
{
    ST_CHECK_PARAM(gbk == NULL, -1);

    if(gbk[0] == 0 || gbk [1] == 0)
    {
        return -1;
    }

    return 2;
}

int get_next_char(const char *token, encoding_type_t encoding)
{
    if(encoding == ENCODING_GBK)
    {
        return get_next_gbk_char(token);
    }
    else if(encoding == ENCODING_UTF8)
    {
        return get_next_utf8_char(token);
    }

    ST_WARNING("Unsupported encoding");
    return -1;
}

//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.
uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 

static unsigned st_rand_state = -1726662223;

int st_rand()
{
    return st_rand_r(&st_rand_state);
}

/* the magic numbers is from stdlibc */
int st_rand_r(unsigned *seed)
{
    *seed = (((*seed) * 1103515245) + 12345) & 0x7fffffff;
    return (int)*seed;
}

void st_srand(unsigned int seed)
{
    st_rand_state = seed;
}

double st_random(double min, double max)
{
    return st_rand() / (double) ST_RAND_MAX *(max - min) + min;
}

void st_shuffle(int *a, size_t n)
{
    size_t i;
    size_t j;
    int t;

    if (n <= 0) {
        return;
    }

    i = n - 1;
    while (i > 1) {
        j = (size_t)((double)i * ( st_rand() / (ST_RAND_MAX + 1.0) ));

        t = a[j];
        a[j] = a[i];
        a[i] = t;
        i--;
    }
}

void st_shuffle_r(int *a, size_t n, unsigned *seed)
{
    size_t i;
    size_t j;
    int t;

    if (n <= 0) {
        return;
    }

    i = n - 1;
    while (i > 1) {
        j = (size_t)((double)i * ( st_rand_r(seed) / (ST_RAND_MAX + 1.0) ));

        t = a[j];
        a[j] = a[i];
        a[i] = t;
        i--;
    }
}

char* st_fgets(char **line, size_t *sz, FILE *fp, bool *err) 
{
    char *ptr;
    size_t old_sz;
    size_t n;

    int ch;

    ST_CHECK_PARAM(line == NULL || sz == NULL || fp == NULL, NULL);

    if (err != NULL) {
        *err = false;
    }

    if (*line == NULL || *sz <= 0) {
        *line = (char *)malloc(MAX_LINE_LEN);
        if (*line == NULL) {
            ST_WARNING("Failed to malloc line");
            goto ERR;
        }
        *sz = MAX_LINE_LEN;
    }

    ptr = *line;
    n = *sz;

    while (--n) {
        if ((ch = getc(fp)) == EOF) {
            if (ptr == *line) {
                return NULL;
            }
            break;
        }

        if ((*ptr++ = ch) == '\n') {
            break;
        }
    }

    if (n > 0) {
        *ptr = '\0';
        return *line;
    }

    do {
        old_sz = *sz;
        *sz *= 2;

        *line = (char *)realloc(*line, *sz);
        if (*line == NULL) {
            ST_WARNING("Failed to realloc line. size[%zu -> %zu]",
                    old_sz, *sz);
            goto ERR;
        }

        ptr = (*line) + old_sz - 1;
        n = old_sz + 1;

        while (--n) {
            if ((ch = getc(fp)) == EOF) {
                break;
            }

            if ((*ptr++ = ch) == '\n') {
                break;
            }
        }
    } while (n == 0);

    *ptr = '\0';

    return *line;
ERR:
    *sz = 0;
    if (err != NULL) {
        *err = true;
    }
    return NULL;
}

int st_readline(FILE *fp, const char *fmt, ...) 
{
    char *line = NULL;
    size_t sz = 0;

    va_list args;
    int ret;

    if (st_fgets(&line, &sz, fp, NULL) == NULL) {
        ST_WARNING("Failed to read line.");
        goto ERR;
    }

    va_start (args, fmt);
    ret = vsscanf(line, fmt, args);
    va_end (args);

    safe_free(line);
    return ret;

ERR:
    safe_free(line);
    return -1;
}

static bool need_quote(const char *str)
{
    const char *ok_chars = "[]~#^_-+=:.,/";
    const char *c = str;
    const char *d;

    if (*c == '\0') {
        return true;  // Must quote empty string
    } else {
        for (; *c != '\0'; c++) {
            // For non-alphanumeric characters we have a list of characters which
            // are OK. All others are forbidden (this is easier since the shell
            // interprets most non-alphanumeric characters).
            if (!isalnum(*c)) {
                for (d = ok_chars; *d != '\0'; d++) {
                    if (*c == *d) {
                        break;
                    }
                }
                // If not alphanumeric or one of the "ok_chars", it must be escaped.
                if (*d == '\0') {
                    return true;
                }
            }
        }

        return false;  // The string was OK. No quoting or escaping.
    }
}

// Returns a quoted and escaped version of "str"
// which has previously been determined to need escaping.
// Our aim is to print out the command line in such a way that if it's
// pasted into a shell, it will get passed to the program in the same way.
static int quote(const char *str, char *ans, size_t ans_len)
{
    // For now we use the following rules:
    // In the normal case, we quote with single-quote "'", and to escape
    // a single-quote we use the string: '\'' (interpreted as closing the
    // single-quote, putting an escaped single-quote from the shell, and
    // then reopening the single quote).
    char quote_char = '\'';
    const char *escape_str = "'\\''";  // e.g. echo 'a'\''b' returns a'b
    size_t n;
    const char *c;
    const char *p;

    // If the string contains single-quotes that would need escaping this
    // way, and we determine that the string could be safely double-quoted
    // without requiring any escaping, then we double-quote the string.
    // This is the case if the characters "`$\ do not appear in the string.
    // e.g. see http://www.redhat.com/mirrors/LDP/LDP/abs/html/quotingvar.html
    if (strchr(str, '\'') && !strpbrk(str, "\"`$\\")) {
        quote_char = '"';
        escape_str = "\\\"";  // should never be accessed.
    }

    n = 0;
    if (n >= ans_len - 1) {
        return -1;
    }
    ans[n] = quote_char;
    n++;

    c = str;
    for (;*c != '\0'; c++) {
        if (*c == quote_char) {
            for(p = escape_str; *p != '\0'; p++) {
                if (n >= ans_len - 1) {
                    return -1;
                }
                ans[n] = *p;
                n++;
            }
        } else {
            if (n >= ans_len - 1) {
                return -1;
            }
            ans[n] = *c;
            n++;
        }
    }
    if (n >= ans_len - 1) {
        return -1;
    }
    ans[n] = quote_char;
    n++;
    ans[n] = 0;

    return 0;
}

int st_escape(const char *str, char *ans, size_t ans_len)
{
    if (need_quote(str)) {
        return quote(str, ans, ans_len);
    } else {
        if (strlen(str) >= ans_len) {
            return -1;
        }
        strcpy(ans, str);
    }

    return 0;
}

int st_escape_args(int argc, const char *argv[], char *ans, size_t ans_len)
{
    size_t len;
    int i;

    ans[0] = 0;
    len = 0;
    for (i = 0; i < argc; i++) {
        if (st_escape(argv[i], ans + len, ans_len - len) < 0) {
            ans[ans_len - 1] = 0;
            return -1;
        }
        len = strlen(ans);
        if (len >= ans_len - 2) {
            break;
        }
        ans[len] = ' ';
        len++;
    }
    ans[ans_len - 1] = 0;

    return 0;
}

off_t st_fsize(const char *filename) 
{
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    ST_WARNING("Cannot determine size of %s: %s\n",
            filename, strerror(errno));

    return -1;
}

double st_gaussrand()
{
    static double V1, V2, S;
    static int phase = 0;
    double X;

    if(phase == 0) {
        do {
            double U1 = (double)st_rand() / ST_RAND_MAX;
            double U2 = (double)st_rand() / ST_RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    return X;
}

double st_normrand(double mean, double stdev)
{
    return ((stdev * st_gaussrand()) + mean);
}

void st_gauss_r_init(st_gauss_r_t *gauss, double mean,
        double stdev, unsigned seed)
{
    memset(gauss, 0, sizeof(st_gauss_r_t));
    gauss->mean = mean;
    gauss->stdev = stdev;
    gauss->seed = seed;
}

double st_gaussrand_r(st_gauss_r_t *gauss)
{
    double U1;
    double U2;
    double X;

    if(gauss->phase == 0) {
        do {
            U1 = (double)st_rand_r(&gauss->seed) / ST_RAND_MAX;
            U2 = (double)st_rand_r(&gauss->seed) / ST_RAND_MAX;

            gauss->V1 = 2 * U1 - 1;
            gauss->V2 = 2 * U2 - 1;
            gauss->S = gauss->V1 * gauss->V1 + gauss->V2 * gauss->V2;
        } while(gauss->S >= 1 || gauss->S == 0);

        X = gauss->V1 * sqrt(-2 * log(gauss->S) / gauss->S);
    } else
        X = gauss->V2 * sqrt(-2 * log(gauss->S) / gauss->S);

    gauss->phase = 1 - gauss->phase;

    return ((gauss->stdev * X) + gauss->mean);
}

/* qsort.c from GNU Libc. http://code.metager.de/source/xref/gnu/glibc/stdlib/qsort.c */
/* Copyright (C) 1991-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Written by Douglas C. Schmidt (schmidt@ics.uci.edu).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* If you consider tuning this algorithm, you should consult first:
   Engineering a sort function; Jon Bentley and M. Douglas McIlroy;
   Software - Practice and Experience; Vol. 23 (11), 1249-1265, 1993.  */

/* Byte-wise swap two items of size SIZE. */
#define SWAP(a, b, size)						      \
  do									      \
    {									      \
      size_t __size = (size);						      \
      char *__a = (a), *__b = (b);					      \
      do								      \
	{								      \
	  char __tmp = *__a;						      \
	  *__a++ = *__b;						      \
	  *__b++ = __tmp;						      \
	} while (--__size > 0);						      \
    } while (0)

/* Discontinue quicksort algorithm when partition gets below this size.
   This particular magic number was chosen to work best on a Sun 4/260. */
#define MAX_THRESH 4

/* Stack node declarations used to store unfulfilled partition obligations. */
typedef struct
  {
    char *lo;
    char *hi;
  } stack_node;

/* The next 4 #defines implement a very fast in-line stack abstraction. */
/* The stack needs log (total_elements) entries (we could even subtract
   log(MAX_THRESH)).  Since total_elements has type size_t, we get as
   upper bound for log (total_elements):
   bits per byte (CHAR_BIT) * sizeof(size_t).  */
#define STACK_SIZE	(CHAR_BIT * sizeof(size_t))
#define PUSH(low, high)	((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define	POP(low, high)	((void) (--top, (low = top->lo), (high = top->hi)))
#define	STACK_NOT_EMPTY	(stack < top)


/* Order size using quicksort.  This implementation incorporates
   four optimizations discussed in Sedgewick:

   1. Non-recursive, using an explicit stack of pointer that store the
      next array partition to sort.  To save time, this maximum amount
      of space required to store an array of SIZE_MAX is allocated on the
      stack.  Assuming a 32-bit (64 bit) integer for size_t, this needs
      only 32 * sizeof(stack_node) == 256 bytes (for 64 bit: 1024 bytes).
      Pretty cheap, actually.

   2. Chose the pivot element using a median-of-three decision tree.
      This reduces the probability of selecting a bad pivot value and
      eliminates certain extraneous comparisons.

   3. Only quicksorts TOTAL_ELEMS / MAX_THRESH partitions, leaving
      insertion sort to order the MAX_THRESH items within each partition.
      This is a big win, since insertion sort is faster for small, mostly
      sorted array segments.

   4. The larger of the two sub-partitions is always pushed onto the
      stack first, with the algorithm then concentrating on the
      smaller partition.  This *guarantees* no more than log (total_elems)
      stack size is needed (actually O(1) in this case)!  */

void st_qsort(void *const pbase, size_t total_elems, size_t size,
	    int (*cmp) (const void *, const void *, void *), void *arg)
{
  char *base_ptr = (char *) pbase;

  const size_t max_thresh = MAX_THRESH * size;

  if (total_elems == 0)
    /* Avoid lossage with unsigned arithmetic below.  */
    return;

  if (total_elems > MAX_THRESH)
    {
      char *lo = base_ptr;
      char *hi = &lo[size * (total_elems - 1)];
      stack_node stack[STACK_SIZE];
      stack_node *top = stack;

      PUSH (NULL, NULL);

      while (STACK_NOT_EMPTY)
        {
          char *left_ptr;
          char *right_ptr;

	  /* Select median value from among LO, MID, and HI. Rearrange
	     LO and HI so the three values are sorted. This lowers the
	     probability of picking a pathological pivot value and
	     skips a comparison for both the LEFT_PTR and RIGHT_PTR in
	     the while loops. */

	  char *mid = lo + size * ((hi - lo) / size >> 1);

	  if ((*cmp) ((void *) mid, (void *) lo, arg) < 0)
	    SWAP (mid, lo, size);
	  if ((*cmp) ((void *) hi, (void *) mid, arg) < 0)
	    SWAP (mid, hi, size);
	  else
	    goto jump_over;
	  if ((*cmp) ((void *) mid, (void *) lo, arg) < 0)
	    SWAP (mid, lo, size);
	jump_over:;

	  left_ptr  = lo + size;
	  right_ptr = hi - size;

	  /* Here's the famous ``collapse the walls'' section of quicksort.
	     Gotta like those tight inner loops!  They are the main reason
	     that this algorithm runs much faster than others. */
	  do
	    {
	      while ((*cmp) ((void *) left_ptr, (void *) mid, arg) < 0)
		left_ptr += size;

	      while ((*cmp) ((void *) mid, (void *) right_ptr, arg) < 0)
		right_ptr -= size;

	      if (left_ptr < right_ptr)
		{
		  SWAP (left_ptr, right_ptr, size);
		  if (mid == left_ptr)
		    mid = right_ptr;
		  else if (mid == right_ptr)
		    mid = left_ptr;
		  left_ptr += size;
		  right_ptr -= size;
		}
	      else if (left_ptr == right_ptr)
		{
		  left_ptr += size;
		  right_ptr -= size;
		  break;
		}
	    }
	  while (left_ptr <= right_ptr);

          /* Set up pointers for next iteration.  First determine whether
             left and right partitions are below the threshold size.  If so,
             ignore one or both.  Otherwise, push the larger partition's
             bounds on the stack and continue sorting the smaller one. */

          if ((size_t) (right_ptr - lo) <= max_thresh)
            {
              if ((size_t) (hi - left_ptr) <= max_thresh)
		/* Ignore both small partitions. */
                POP (lo, hi);
              else
		/* Ignore small left partition. */
                lo = left_ptr;
            }
          else if ((size_t) (hi - left_ptr) <= max_thresh)
	    /* Ignore small right partition. */
            hi = right_ptr;
          else if ((right_ptr - lo) > (hi - left_ptr))
            {
	      /* Push larger left partition indices. */
              PUSH (lo, right_ptr);
              lo = left_ptr;
            }
          else
            {
	      /* Push larger right partition indices. */
              PUSH (left_ptr, hi);
              hi = right_ptr;
            }
        }
    }

  /* Once the BASE_PTR array is partially sorted by quicksort the rest
     is completely sorted using insertion sort, since this is efficient
     for partitions below MAX_THRESH size. BASE_PTR points to the beginning
     of the array to sort, and END_PTR points at the very last element in
     the array (*not* one beyond it!). */

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

  {
    char *const end_ptr = &base_ptr[size * (total_elems - 1)];
    char *tmp_ptr = base_ptr;
    char *thresh = min(end_ptr, base_ptr + max_thresh);
    char *run_ptr;

    /* Find smallest element in first threshold and place it at the
       array's beginning.  This is the smallest array element,
       and the operation speeds up insertion sort's inner loop. */

    for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
      if ((*cmp) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
        tmp_ptr = run_ptr;

    if (tmp_ptr != base_ptr)
      SWAP (tmp_ptr, base_ptr, size);

    /* Insertion sort, running from left-hand-side up to right-hand-side.  */

    run_ptr = base_ptr + size;
    while ((run_ptr += size) <= end_ptr)
      {
	tmp_ptr = run_ptr - size;
	while ((*cmp) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
	  tmp_ptr -= size;

	tmp_ptr += size;
        if (tmp_ptr != run_ptr)
          {
            char *trav;

	    trav = run_ptr + size;
	    while (--trav >= run_ptr)
              {
                char c = *trav;
                char *hi, *lo;

                for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                  *hi = *lo;
                *hi = c;
              }
          }
      }
  }
}

/* end of qsort.c */

static int st_allrange(void *base, size_t i, size_t n, size_t sz,
        int (*callback)(void *base, size_t n, void *args), void *args)
{
    size_t k;

    if (i == n - 1) {
        return callback(base, n, args);
    }

    for (k = i; k < n; k++) {
        SWAP((char *)base + i * sz, (char *)base + k * sz, sz);
        if (st_allrange(base, i + 1, n, sz, callback, args) < 0) {
            return -1;
        }
        SWAP((char *)base + i * sz, (char *)base + k * sz, sz);
    }

    return 0;
}

int st_permutation(void *base, size_t n, size_t sz,
        int (*callback)(void *base, size_t n, void *args), void *args)
{
    return st_allrange(base, 0, n, sz, callback, args);
}

int st_parse_int_array(const char *line, int **arr, int *n_arr)
{
    const char *q;
    bool neg;
    int n;

    q = line;
    n = INT_MAX;
    neg = false;

    while (*q != 0) {
        if (*q == ',') {
            if (n == INT_MAX) {
                ST_WARNING("Error array: No number found before ','");
                return -1;
            }
            *arr = (int *)realloc(*arr, sizeof(int)*(*n_arr + 1));
            if (*arr == NULL) {
                ST_WARNING("Failed to realloc array[%d].", *n_arr);
                return -1;
            }
            if (neg) {
                n = -n;
            }
            (*arr)[*n_arr] = n;
            ++(*n_arr);
            n = INT_MAX;
            neg = false;
        } else {
            if (*q == '-') {
                if (n != INT_MAX) {
                    ST_WARNING("Error array: found not proper '-'");
                    return -1;
                }
                neg = true;
            } else if (*q < '0' || *q > '9') {
                ST_WARNING("Error array: found non-digit[%c]", *q);
                return -1;
            } else {
                if (n == INT_MAX) {
                    n = (*q - '0');
                } else {
                    n = n * 10 + (*q - '0');
                }
            }
        }

        ++q;
    }
    if (n == INT_MAX) {
        ST_WARNING("Error array: extra ',' found in the end");
        return -1;
    }
    *arr = (int *)realloc(*arr, sizeof(int)*(*n_arr + 1));
    if (*arr == NULL) {
        ST_WARNING("Failed to realloc array[%d].", *n_arr);
        return -1;
    }
    if (neg) {
        n = -n;
    }
    (*arr)[*n_arr] = n;
    ++(*n_arr);

    return 0;
}
