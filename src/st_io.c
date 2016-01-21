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

#include <sys/stat.h>

#include <stutils/st_macro.h>
#include "st_log.h"
#include "st_io.h"

FILE* st_fopen(const char *name, const char *mode)
{
    ST_CHECK_PARAM(name == NULL || mode == NULL, NULL);

    if (name[0] == '\0' || (name[0] == '-' && name[1] == '\0')) {
        if (mode[0] == 'r') {
            return stdin;
        } else if (mode[0] == 'w' || mode[0] == 'a') {
            return stdout;
        } else {
            ST_WARNING("Unkown mode[%s].", mode);
            return NULL;
        }
    } else if (name[0] == '|') {
        return popen(name, mode);
    } else {
        return fopen(name, mode);
    }
}

void st_fclose(FILE *fp)
{
    struct stat s;

    if (fp == stdin || fp == stdout) {
        return;
    }

    if(fstat(fileno(fp), &s) != 0) {
        ST_WARNING("Failed to fstat[%m]");
        return;
    }

    if ((s.st_mode & S_IFMT) == S_IFIFO) {
        pclose(fp);
    } else {
        fclose(fp);
    }
}
