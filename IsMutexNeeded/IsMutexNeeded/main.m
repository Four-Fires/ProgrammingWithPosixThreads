//
//  main.cpp
//  IsMutexNeeded
//
//  Created by matthew on 2013-12-27.
//  Copyright (c) 2013 matthew. All rights reserved.
//

#define USING_MUTEX     0

#include "errors.h"
#include <pthread.h>

long sharedNum;
long total;
pthread_mutex_t int_rw_mutex = PTHREAD_MUTEX_INITIALIZER;

void *int_reading_thread (void *arg)
{
    for (long i=0; i<1000000; i++) {
#if USING_MUTEX
        pthread_mutex_lock(&int_rw_mutex);
#endif
        for (int j=0; j<10; j++)
        {
            long tmp = sharedNum;
        }
#if USING_MUTEX
        pthread_mutex_unlock(&int_rw_mutex);
#endif
    }
    return NULL;
}

void *int_increase_thread (void *arg)
{
    for (long i=0; i<1000000; i++) {
#if USING_MUTEX
        pthread_mutex_lock(&int_rw_mutex);
#endif
        sharedNum++;
        total+=sharedNum;
#if USING_MUTEX
        pthread_mutex_unlock(&int_rw_mutex);
#endif
    }
    return NULL;
}

void *int_decrease_thread (void *arg)
{
    for (long i=0; i<1000000; i++) {
#if USING_MUTEX
        pthread_mutex_lock(&int_rw_mutex);
#endif
        sharedNum--;
        total+=sharedNum;
#if USING_MUTEX
        pthread_mutex_unlock(&int_rw_mutex);
#endif
    }
    return NULL;
}

int main(int argc, const char * argv[])
{
    int status;
    pthread_t thread_r;
    pthread_t thread_inc;
    pthread_t thread_dec;
    
    // test 1, write/write
    sharedNum = 0;
    total = 0;
    status = pthread_create (
                             &thread_inc, NULL, int_increase_thread, NULL);
    if (status != 0)
        err_abort (status, "Create thread_inc");
    
    status = pthread_create (
                             &thread_dec, NULL, int_decrease_thread, NULL);
    if (status != 0)
        err_abort (status, "Create thread_dec");
    
    pthread_join(thread_inc, NULL);
    pthread_join(thread_dec, NULL);
    
    printf("inc,dec results:\n");
    printf("sharedNum: %ld\n\n", sharedNum);
    
    // test 2, read/write
    sharedNum = 0;
    total = 0;
    status = pthread_create (
                             &thread_inc, NULL, int_increase_thread, NULL);
    if (status != 0)
        err_abort (status, "Create thread_inc");
    
    status = pthread_create (
                             &thread_r, NULL, int_reading_thread, NULL);
    if (status != 0)
        err_abort (status, "Create thread_r");
    
    pthread_join(thread_inc, NULL);
    pthread_join(thread_r, NULL);
    
    printf("read,inc results:\n");
    printf("sharedNum: %ld\n", sharedNum);
    printf("total: %ld\n", total);
}

