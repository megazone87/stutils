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

#include "st_log.h"
#include "st_semaphore.h"

int st_sem_init(st_sem_t *sem, int init_value)
{
    sem->value = init_value;

    if (pthread_mutex_init(&sem->mutex, NULL) != 0) {
        ST_WARNING("Cannot initialize pthread mutex");
        return -1;
    }
    if (pthread_cond_init(&sem->cond, NULL) != 0) {
        ST_WARNING("Cannot initialize pthread condition variable");
        return -1;
    }

    return 0;
}

int st_sem_destroy(st_sem_t *sem)
{
    int ret = 0;

    ret |= pthread_mutex_destroy(&sem->mutex);
    ret |= pthread_cond_destroy(&sem->cond);

    return ret;
}

int st_sem_wait(st_sem_t *sem)
{
    int ret = 0;
    ret |= pthread_mutex_lock(&sem->mutex);
    while (sem->value <= 0) {
        ret |= pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    sem->value--;
    ret |= pthread_mutex_unlock(&sem->mutex);
    if (ret != 0) {
        ST_WARNING("Error in pthreads");
        return ret;
    }

    return 0;
}

int st_sem_post(st_sem_t *sem)
{
    int ret = 0;

    ret |= pthread_mutex_lock(&sem->mutex);
    sem->value++;
    ret |= pthread_cond_signal(&sem->cond);
    ret |= pthread_mutex_unlock(&sem->mutex);
    if (ret != 0) {
        ST_WARNING("Error in pthreads");
        return ret;
    }

    return 0;
}

int st_sem_getvalue(st_sem_t *sem, int *val)
{
    *val = sem->value;

    return 0;
}
