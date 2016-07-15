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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/stat.h>

#include <stutils/st_macro.h>
#include "st_log.h"
#include "st_string.h"

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

const char* get_next_token(const char *line, char *token)
{
    while(*line && (*line == ' ' || *line == '\t'
                    || *line == '\r' || *line == '\n')) {
        line++;
    }

    while(*line && *line != ' ' && *line != '\t'
            && *line != '\r' && *line != '\n') {
        *token = *line;
        token++;
        line++;
    }

    *token = 0;
    if(*line == 0) {
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

int st_str_replace(char* res, size_t res_len,
        const char* src, const char* from, const char* to,
        int max_num)
{
    const char *str, *p;
    size_t sz;
    int num;

    ST_CHECK_PARAM(res == NULL || res_len <= 0
            || src == NULL || from == NULL, -1);

    str = src;
    num = 0;
    sz = 0;

    while (max_num <= 0 || num < max_num) {
        p = strstr(str, from);
        if (p == NULL) {
            break;
        }

        if (sz + p - str >= res_len) {
            ST_WARNING("not enough space for result.");
            goto ERR;
        }
        strncpy(res + sz, str, p - str);
        sz += p - str;

        if (to != NULL) {
            if (sz + strlen(to) >= res_len) {
                ST_WARNING("not enough space for result.");
                goto ERR;
            }
            strcpy(res + sz, to);
            sz += strlen(to);
        }
        ++num;
        str = p + strlen(from);
    }
    if (sz + strlen(str) >= res_len) {
        ST_WARNING("not enough space for result.");
        goto ERR;
    }
    strcpy(res + sz, str);

    return num;

ERR:
    res[sz] = '\0';
    return -1;
}

long long st_str2ll(const char *str)
{
    long long l;
    size_t len;

    l = atoll(str);

    if (l == 0) {
        return l;
    }

    len = strlen(str);

    if (len < 1) {
        return l;
    }
    switch (str[len - 1]) {
        /* All fall through */
        case 'Y':
            l *= 1000;
        case 'Z':
            l *= 1000;
        case 'E':
            l *= 1000;
        case 'P':
            l *= 1000;
        case 'T':
            l *= 1000;
        case 'G':
            l *= 1000;
        case 'M':
            l *= 1000;
        case 'k':
            l *= 1000;
            break;
        case 'i':
            if (len < 2) {
                return l;
            }

            switch (str[len - 2]) {
                /* All fall through */
                case 'Y':
                    l *= 1024;
                case 'Z':
                    l *= 1024;
                case 'E':
                    l *= 1024;
                case 'P':
                    l *= 1024;
                case 'T':
                    l *= 1024;
                case 'G':
                    l *= 1024;
                case 'M':
                    l *= 1024;
                case 'K':
                    l *= 1024;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return l;
}

char* st_ll2str(char *str, size_t len, long long l, bool binary)
{
    static char suf[] = {'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};
    long long t;
    long long base;
    size_t sz;
    int n;

    ST_CHECK_PARAM(str == NULL || len <= 0, NULL);

    t = l;
    n = 0;
    if (binary) {
        base = 1024;
    } else {
        base = 1000;
    }
    while (t != 0 && t % base == 0) {
        t = t / base;
        n++;
        if (n >= sizeof(suf)) {
            break;
        }
    }

    snprintf(str, len, "%lld", t);

    if (n > 0) {
        sz = strlen(str);
        if (binary) {
            if (sz > len - 3) {
                ST_WARNING("string buf len not enough");
                return NULL;
            }
            str[sz] = toupper(suf[n - 1]);
            str[sz+1] = 'i';
            str[sz+2] = '\0';
        } else {
            if (sz > len - 2) {
                ST_WARNING("string buf len not enough");
                return NULL;
            }
            str[sz] = suf[n - 1];
            str[sz+1] = '\0';
        }
    }

    return str;
}

char* st_strncatf(char *dst, size_t len, const char *fmt, ...)
{
    char buf[MAX_LINE_LEN];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, MAX_LINE_LEN, fmt, args);
    va_end(args);

    strncat(dst, buf, len - strlen(buf) - 1);

    return dst;
}
