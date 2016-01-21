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

#ifndef  _ST_SEMAPHORE_H_
#define  _ST_SEMAPHORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include <stutils/st_macro.h>

/** @defgroup semaphore Semaphore
 * Data structures and functions for Semaphore.
 */

/**
 * Semaphore.
 * @ingroup semaphore
 */
typedef struct _st_sem_t_ {
  int value; /**< the semaphore counter. 0 means block on st_sem_wait(). */
  int inited; /**< indicate whether it is initialized. using to avoid double destroy. */
  
  pthread_mutex_t mutex; /**< condition variable lock. */
  pthread_cond_t cond; /**< condition variable. */
} st_sem_t;


/**
 * Initialize a semaphore
 * @ingroup semaphore
 * @param[out] sem semaphore to be initialised.
 * @param[in] init_value initial value of the semaphore.
 * @return non-zero value if any error.
 */
int st_sem_init(st_sem_t *sem, int init_value);

/**
 * Destroy a semaphore
 * @ingroup semaphore
 * @param[in] sem semaphore to be destroyed.
 * @return non-zero value if any error.
 */
int st_sem_destroy(st_sem_t *sem);

/**
 * Lock a semaphore
 * @ingroup semaphore
 * @param[in] sem semaphore to be locked.
 * @return non-zero value if any error.
 */
int st_sem_wait(st_sem_t *sem);
/**
 * Unlock a semaphore
 * @ingroup semaphore
 * @param[in] sem semaphore to be unlocked.
 * @return non-zero value if any error.
 */
int st_sem_post(st_sem_t *sem);

/**
 * get the value of a semaphore
 * @ingroup semaphore
 * @param[in] sem the semaphore.
 * @param[out] val value of the semaphore.
 * @return non-zero value if any error.
 */
int st_sem_getvalue(st_sem_t *sem, int *val);

#ifdef __cplusplus
}
#endif

#endif
