//
//  main.cpp
//  IsMutexNeeded
//
//  Created by matthew on 2013-12-27.
//  Copyright (c) 2013 matthew. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "errors.h"
#include <pthread.h>

//bool sharedBool;
BOOL sharedBool;
NSNumber *sharedNum;

void *bool_reading_thread (void *arg)
{
    while (1) {
//        bool curBool = sharedBool;
        BOOL curBool = sharedBool;

        if (curBool)
            printf("curBool: true\n");
        else
            printf("curBool: false\n");
    }
}

void *bool_writing_thread (void *arg)
{
    while (1) {
        sharedBool = !sharedBool;
        if (sharedBool)
            printf("sharedBool: true\n");
        else
            printf("sharedBool: false\n");
    }
}

void *int_reading_thread (void *arg)
{
    while (1) {
        NSLog(@"int is: %@", sharedNum);
    }
}

void *int_increase_thread (void *arg)
{
    while (1) {
        sharedNum = [NSNumber numberWithInt:[sharedNum intValue]+1];
    }
}

void *int_decrease_thread (void *arg)
{
    while (1) {
        sharedNum = [NSNumber numberWithInt:[sharedNum intValue]-1];
    }
}

int main(int argc, const char * argv[])
{
    int status;
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    pthread_t thread5;
    
    sharedBool = false;
    sharedNum = [NSNumber numberWithInt:0];
    
//    status = pthread_create (
//                             &thread1, NULL, bool_writing_thread, NULL);
//    if (status != 0)
//        err_abort (status, "Create thread1");
//
//    status = pthread_create (
//                             &thread2, NULL, bool_writing_thread, NULL);
//    if (status != 0)
//        err_abort (status, "Create thread2");
//    
//    status = pthread_create (
//                             &thread3, NULL, int_increase_thread, NULL);
//    if (status != 0)
//        err_abort (status, "Create thread3");
//    
//    status = pthread_create (
//                             &thread4, NULL, int_decrease_thread, NULL);
//    if (status != 0)
//        err_abort (status, "Create thread4");
//    
//    status = pthread_create (
//                             &thread5, NULL, int_reading_thread, NULL);
//    if (status != 0)
//        err_abort (status, "Create thread5");

    for (int i=0; i<50000; i++) {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            sharedNum = [NSNumber numberWithInt:[sharedNum intValue]+1];
            NSLog(@"increase %d", i);
        });
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            sharedNum = [NSNumber numberWithInt:[sharedNum intValue]-1];
            NSLog(@"decrease %d", i);
        });
    }
    
    pthread_exit(NULL);
}

