//
//  main.cpp
//  IsMutexNeeded
//
//  Created by matthew on 2013-12-27.
//  Copyright (c) 2013 matthew. All rights reserved.
//

#include "errors.h"
#include <pthread.h>

bool sharedBool;

void *reading_thread (void *arg)
{
    while (1) {
        bool curBool = sharedBool;
        if (curBool)
            printf("curBool: true\n");
        else
            printf("curBool: false\n");
    }
}

void *writing_thread (void *arg)
{
    while (1) {
        sharedBool = !sharedBool;
        if (sharedBool)
            printf("sharedBool: true\n");
        else
            printf("sharedBool: false\n");
    }
}

int main(int argc, const char * argv[])
{
    int status;
    pthread_t thread1;
    pthread_t thread2;
    
    sharedBool = false;
    
    status = pthread_create (
                             &thread1, NULL, writing_thread, NULL);
    if (status != 0)
        err_abort (status, "Create thread1");

    status = pthread_create (
                             &thread2, NULL, writing_thread, NULL);
    if (status != 0)
        err_abort (status, "Create thread2");

    pthread_exit(NULL);
}

