//
//  main.cpp
//  Ch4PipeNew
//
//  Created by matthew on 2014-04-05.
//  Copyright (c) 2014 matthew. All rights reserved.
//

#include <pthread.h>
#include "errors.h"

#include <queue>
#include <string>
#include <iostream>

const unsigned int NUM_OF_THREADS = 100;
const unsigned long WORKLOAD = 10000000;


struct data_to_process
{
    int index;
    int value;
};

struct thread_context
{
    unsigned long workload;
    
    pthread_mutex_t in_mutex;
    pthread_cond_t in_avail_cond;
    pthread_cond_t in_empty_cond;
    data_to_process *pInData;
    
    pthread_mutex_t *pOutMutex;
    pthread_cond_t *pOutAvailCond;
    pthread_cond_t *pOutEmptyCond;
    data_to_process **ppOutData;
};

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void debugLog(std::string log)
{
    pthread_mutex_lock(&log_mutex);
    time_t now;
    time(&now);
    std::cout /*<< ctime(&now)*/ << " " << pthread_self() << " "<< log << std::endl;
    pthread_mutex_unlock(&log_mutex);
}

void *stage_thread (void *arg);

int main(int argc, const char * argv[])
{
    debugLog("main thread started");

    // insert code here...
    struct thread_context threadContext[NUM_OF_THREADS];
    for (int i=0; i<NUM_OF_THREADS; i++)
    {
        threadContext[i].workload = WORKLOAD;
        
        threadContext[i].in_mutex = PTHREAD_MUTEX_INITIALIZER;
        threadContext[i].in_avail_cond = PTHREAD_COND_INITIALIZER;
        threadContext[i].in_empty_cond = PTHREAD_COND_INITIALIZER;
        threadContext[i].pInData = NULL;
        
        threadContext[i].pOutMutex = NULL;
        threadContext[i].pOutAvailCond = NULL;
        threadContext[i].pOutEmptyCond = NULL;
        threadContext[i].ppOutData = NULL;
    }
    
    for (int i=0; i<NUM_OF_THREADS-1; i++)
    {
        threadContext[i].pOutMutex = &(threadContext[i+1].in_mutex);
        threadContext[i].pOutAvailCond = &(threadContext[i+1].in_avail_cond);
        threadContext[i].pOutEmptyCond = &(threadContext[i+1].in_empty_cond);
        threadContext[i].ppOutData = &(threadContext[i+1].pInData);
    }
    
    pthread_t thread;
    for (int i=0; i<NUM_OF_THREADS; i++)
    {
        int status = pthread_create (&thread, NULL, stage_thread, &threadContext[i]);
        
        if (status != 0)
            err_abort (status, "Create thread");
    }
    
    for (int i = 1; i<=100; i++)
    {
        struct data_to_process* pInputData = new struct data_to_process();
        pInputData->index = i;
        pInputData->value = i;
        
        debugLog("locking out_mutex...");
        pthread_mutex_lock(&threadContext[0].in_mutex);
        debugLog("locked out_mutex");
        while (threadContext[0].pInData!=NULL)
        {
            debugLog("downstream not ready yet, waiting for out_empty_cond...");
            pthread_cond_wait(&threadContext[0].in_empty_cond, &threadContext[0].in_mutex);
            debugLog("out_empty_cond siganled");
        }
        threadContext[0].pInData = pInputData;
        pthread_cond_signal(&threadContext[0].in_avail_cond);
        debugLog("signaled out_avail_cond");
        pthread_mutex_unlock(&threadContext[0].in_mutex);
        debugLog("unlocked out_mutex");
    }
    
    while (true);
    
    return 0;
}

void *stage_thread (void *arg)
{
    debugLog("thread started");
    
    struct thread_context *pMyContext = (struct thread_context*)arg;
    
    while (true)
    {
        data_to_process *pData = NULL;
        debugLog("locking in_mutex...");
        pthread_mutex_lock(&pMyContext->in_mutex);
        debugLog("locked in_mutex");
        while (pMyContext->pInData==NULL)
        {
            debugLog("no data yet, waiting for in_avail_cond...");
            pthread_cond_wait(&pMyContext->in_avail_cond, &pMyContext->in_mutex);
            debugLog("in_avail_cond signaled");
        }
        pData = pMyContext->pInData;
        pMyContext->pInData = NULL;
        pthread_cond_signal(&pMyContext->in_empty_cond);
        pthread_mutex_unlock(&pMyContext->in_mutex);
        debugLog("unlocked in_mutex");

        debugLog("start processing...");
        pthread_mutex_lock(&log_mutex);
        std::cout << "index: " << pData->index << std::endl;
        pthread_mutex_unlock(&log_mutex);

        for (unsigned long i=0; i<pMyContext->workload; i++)
        {
            
            pData->value += 1;
            pData->value -= 1;
        }
        pData->value++;
        debugLog("done processing");
        
        if (pMyContext->ppOutData == NULL) {
            debugLog("final result");
            pthread_mutex_lock(&log_mutex);
            std::cout << pData->value << std::endl;
            pthread_mutex_unlock(&log_mutex);

            delete pData;
        }
        else {
            debugLog("locking out_mutex...");
            pthread_mutex_lock(pMyContext->pOutMutex);
            debugLog("locked out_mutex");
            while (*(pMyContext->ppOutData)!=NULL)
            {
                debugLog("downstream not ready yet, waiting for out_empty_cond...");
                pthread_cond_wait(pMyContext->pOutEmptyCond, pMyContext->pOutMutex);
                debugLog("out_empty_cond siganled");
            }
            *(pMyContext->ppOutData) = pData;
            pthread_cond_signal(pMyContext->pOutAvailCond);
            debugLog("signaled out_avail_cond");
            pthread_mutex_unlock(pMyContext->pOutMutex);
            debugLog("unlocked out_mutex");
        }
    }
    return NULL;
}