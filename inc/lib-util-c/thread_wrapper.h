// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#ifndef _WIN32
#include <pthread.h>
#else
#include <windows.h>

#include "umock_c/umock_c_prod.h"

typedef CRITICAL_SECTION pthread_mutex_t;
typedef void pthread_mutexattr_t;
typedef void pthread_condattr_t;
typedef void pthread_rwlockattr_t;
typedef HANDLE pthread_t;
typedef CONDITION_VARIABLE pthread_cond_t;

typedef struct {
    SRWLock lock;
    bool    exclusive;
} pthread_rwlock_t;

struct timespec {
    long tv_sec;
    long tv_nsec;
};

int pthread_create(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **value_ptr);
int pthread_detach(pthread_t);

int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

int pthread_cond_init(thread_cond_t *cond, pthread_condattr_t *attr);
int pthread_cond_destroy(thread_cond_t *cond);
int pthread_cond_wait(thread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(thread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_cond_signal(thread_cond_t *cond);
int pthread_cond_broadcast(thread_cond_t *cond);

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t  *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

int nanosleep(const struct timespec *req, struct timespec *rem);
#endif

unsigned int pcthread_get_num_procs();
void ms_to_timespec(struct timespec* ts, unsigned int ms);

#ifdef __cplusplus
}
#endif
