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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "st_io.h"
#include "st_conf.h"

#define MAX_SEC 8
#define MAX_ITEM 16
typedef struct _item_t_ {
    char key[MAX_ST_CONF_LEN];
    char value[MAX_ST_CONF_LEN];
} item_t;

typedef struct _sec_ref_t_ {
    char name[MAX_ST_CONF_LEN];
    bool file_conf;

    item_t items[MAX_ITEM];
    int num_item;
} sec_ref_t;

typedef struct _ref_t_ {
    sec_ref_t def_sec;

    sec_ref_t secs[MAX_SEC];
    int num_sec;
} ref_t;

static char files[MAX_SEC + 2][MAX_DIR_LEN];
static int num_file = 0;

static const char* add_file()
{
    int fd;

    char template[] = "/tmp/st-conf-XXXXXX";
    fd = mkstemp(template);
    assert(fd != -1);
    safe_close(fd);
    strncpy(files[num_file], template, MAX_DIR_LEN);
    files[num_file][MAX_DIR_LEN - 1] = '\0';
    num_file++;

    return files[num_file - 1];
}

static void save_sec_item(sec_ref_t *sec_ref, FILE *fp)
{
    int i;

    assert(sec_ref != NULL && fp != NULL);

    for (i = 0; i < sec_ref->num_item; i++) {
        fprintf(fp, "%s : %s\n", sec_ref->items[i].key,
                sec_ref->items[i].value);
    }
}

static void save_sec(sec_ref_t *sec_ref, FILE *fp)
{
    const char *file;
    FILE *sub_fp = NULL;

    if (sec_ref->file_conf) {
        file = add_file();
        sub_fp = st_fopen(file, "w");
        assert(sub_fp != NULL);

        save_sec_item(sec_ref, sub_fp);

        safe_st_fclose(sub_fp);

        fprintf(fp, "[%s:%s]\n", sec_ref->name, file);
    } else {
        if (sec_ref->name[0] != '\0') {
            fprintf(fp, "[%s]\n", sec_ref->name);
        }
        save_sec_item(sec_ref, fp);
    }
}

static const char* mk_conf_files(ref_t *ref)
{
    int i;
    const char *file;
    FILE* fp = NULL;

    file = add_file();
    fp = st_fopen(file, "w");
    assert(fp != NULL);

    save_sec(&ref->def_sec, fp);

    for (i = 0; i < ref->num_sec; i++) {
        save_sec(ref->secs + i, fp);
    }

    safe_st_fclose(fp);

#ifdef _ST_CONF_TEST_DEBUG_
    fprintf(stderr, "Main conf: %s\n", file);
#endif

    return file;
}

static void clean_conf_files()
{
    int i;

    for (i = 0; i < num_file; i++) {
        if (files[i][0] != '\0') {
#ifndef _ST_CONF_TEST_DEBUG_
            remove(files[i]);
#endif
            files[i][0] = '\0';
        }
    }
    num_file = 0;
}

static int check_sec(st_conf_t *conf, sec_ref_t *sec_ref)
{
    char value[MAX_ST_CONF_LEN];
    int i;

    for (i = 0; i < sec_ref->num_item; i++) {
        if (st_conf_get_str(conf, sec_ref->name,
                sec_ref->items[i].key, value,
                MAX_ST_CONF_LEN, NULL) < 0) {
            fprintf(stderr, "item[%d] not match\n", i);
            return -1;
        }

        if (strcasecmp(value, sec_ref->items[i].value) != 0) {
            fprintf(stderr, "value[%d] not match\n", i);
            return -1;
        }
    }

    return 0;
}

static int check_conf(st_conf_t *conf, ref_t *ref)
{
    int i;

    if (check_sec(conf, &ref->def_sec) != 0) {
        fprintf(stderr, "def_sec not match\n");
        return -1;
    }

    for (i = 0; i < ref->num_sec; i++) {
        if (check_sec(conf, ref->secs + i) != 0) {
            fprintf(stderr, "sec[%d] not match\n", i);
            return -1;
        }
    }

    return 0;
}

static int unit_test_load()
{
    st_conf_t *conf = NULL;
    const char *file;
    int ncase;
    ref_t ref;
    ref_t std_ref = {
        .def_sec = {"", false, {{"def_key1", "def_val1"},
                                {"def_key2", "def_val2"},
                                {"def_key3", "def_val3"}}, 3},

        .secs = {{"sec", false, {{"sec_key1", "sec_val1"}}, 1},
                 {"sec/sub", false, {{"sub_key1", "sub_val1"},
                                     {"sub_key2", "sub_val2"}}, 2},
                 {"sec/sub/sub1", false, {{"ss1_key1", "ss2_val1"},
                                         {"ss1_key2", "ss2_val2"}}, 2},
                 {"sec/sub/sub2", false, {{"ss2_key1", "ss2_val1"},
                                         {"ss2_key2", "ss2_val2"}}, 2},
                },
        .num_sec = 4,
    };

    fprintf(stderr, " Testing st_conf_load...\n");

    ncase = 1;
    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    ref = std_ref;
    file = mk_conf_files(&ref);
    assert(file != NULL);
    conf = st_conf_create();
    assert (conf != NULL);
    if (st_conf_load(conf, file) < 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }

    if (check_conf(conf, &ref) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    clean_conf_files();
    safe_st_conf_destroy(conf);
    fprintf(stderr, "Passed\n");

    /*****************************************/
    fprintf(stderr, "    Case %d...", ncase++);
    ref = std_ref;
    ref.secs[1].file_conf = true;
    file = mk_conf_files(&ref);
    assert(file != NULL);
    conf = st_conf_create();
    assert (conf != NULL);
    if (st_conf_load(conf, file) < 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }

    if (check_conf(conf, &ref) != 0) {
        fprintf(stderr, "Failed\n");
        goto FAILED;
    }
    clean_conf_files();
    safe_st_conf_destroy(conf);
    fprintf(stderr, "Passed\n");
    return 0;

FAILED:
    clean_conf_files();
    safe_st_conf_destroy(conf);
    return -1;
}

static int run_all_tests()
{
    int ret = 0;

    if (unit_test_load() != 0) {
        ret = -1;
    }

    return ret;
}

int main(int argc, const char *argv[])
{
    int ret;

    fprintf(stderr, "Start testing...\n");
    ret = run_all_tests();
    if (ret != 0) {
        fprintf(stderr, "Tests failed.\n");
    } else {
        fprintf(stderr, "Tests succeeded.\n");
    }

    return ret;
}
