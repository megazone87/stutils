/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Wang Jian
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

#ifndef  _ST_STRING_H_
#define  _ST_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include <stutils/st_macro.h>

void remove_newline(char *line);

void remove_leading_space(char *line);

void trim(char *line);

int split_line(const char *line, char *fields,
        int n_field, int field_len, const char *seps);

typedef enum _encoding_type_t_
{
    ENCODING_GBK  = 0,
    ENCODING_UTF8 = 1,
} encoding_type_t;

int get_next_char(const char *token, encoding_type_t encoding);

const char* get_next_token(const char *line, char *token);

int st_escape(const char *str, char *ans, size_t ans_len);

int st_escape_args(int argc, const char *argv[], char *ans, size_t ans_len);

/**
 * Replace 'max_num' ocurrences of 'from' in 'src' to 'to',
 * storing result to 'res'.
 *
 * @return number of ocurrences replaced.
 */
int st_str_replace(char* res, size_t res_len,
        const char* src, const char* from, const char* to,
        int max_num);

/**
 * Convert string to long long int.
 * Support k,M,G,T... or Ki,Mi,Gi,Ti... suffixes.
 *
 * @param[in] str string format.
 * @return converted number.
 */
long long st_str2ll(const char *str);

/**
 * Convert long long int to string.
 * Support k,M,G,T... or Ki,Mi,Gi,Ti... suffixes.
 *
 * @param[out] str string buffer.
 * @param[in] len length of string buffer.
 * @param[in] l long long int.
 * @param[in] binary base-1024 if true, otherwise base-1000.
 * @return NULL on error, otherwise the string buffer.
 */
char* st_ll2str(char *str, size_t len, long long l, bool binary);

/**
 * Cancat a str with format.
 *
 * @param[in] dst dst string buffer.
 * @param[in] len length of dst.
 * @param[in] fmt format string.
 * @return NULL on error, otherwise the string buffer.
 */
char* st_strncatf(char *dst, size_t len, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
