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
#include "st_macro.h"
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
 
int split_line(const char *line, char fields[][MAX_LINE_LEN], 
        int max_field, const char *sep)
{
    char *pstr;
    int i = 0;

    ST_CHECK_PARAM(fields == NULL || line == NULL, -1);

    pstr = strtok((char *)line, sep);
    if(NULL == pstr)
    {
        strncpy(fields[0], line, MAX_LINE_LEN);
        return 1;
    }
    strncpy(fields[i++], pstr, MAX_LINE_LEN);

    while((pstr = strtok(NULL, sep)) != NULL && i < max_field)
    {
        strncpy(fields[i++], pstr, MAX_LINE_LEN);
    }

    if(pstr)
    {
        ST_WARNING("Too many fields[%d/%d]", i, max_field);
        return -1;
    }

    return i;
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

char* get_next_token(char *line, char *token)
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

void st_shuffle(int *a, size_t n)
{
    size_t i = n - 1;
    size_t j;
    int t;

    while (i > 1) {
        j = (size_t)((double)i * ( rand() / (RAND_MAX + 1.0) ));

        t = a[j];
        a[j] = a[i];
        a[i] = t;
        i--;
    }
}

void st_shuffle_r(int *a, size_t n, unsigned *rand)
{
    size_t i = n - 1;
    size_t j;
    int t;

    while (i > 1) {
        j = (size_t)((double)i * ( rand_r(rand) / (RAND_MAX + 1.0) ));

        t = a[j];
        a[j] = a[i];
        a[i] = t;
        i--;
    }
}

