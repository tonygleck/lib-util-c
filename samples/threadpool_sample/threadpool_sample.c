
#include <stdio.h>

#include "lib-util-c/thread_mgr.h"
#include "lib-util-c/threadpool.h"
#include "lib-util-c/condition_mgr.h"

#include "lib-util-c/mutex_mgr.h"

SIGNAL_HANDLE signal;
MUTEX_HANDLE mutex;

void threadpool_start_func(void* parameter)
{
    unsigned int* thread_num = (unsigned int*)parameter;
    printf("Start %u\n", *thread_num);
    thread_mgr_sleep(5000);
    printf("End %u\n", *thread_num);
}

void thread_start_func(void* parameter)
{
    printf("Running Thread\n");
    thread_mgr_sleep(10000);
    printf("Done running thread\n");
    condition_mgr_signal(signal);
    //condition_mgr_broadcast(signal);
    printf("Exiting thread\n");
}

int main()
{
    unsigned int thread_num = 0;

    mutex_mgr_create(&mutex);
    condition_mgr_init(&signal);


    THREAD_MGR_HANDLE thr_mgr = thread_mgr_init(thread_start_func, &thread_num);
    thread_mgr_detach(thr_mgr);

    condition_mgr_wait(signal, mutex);

    mutex_mgr_destroy(mutex);
    condition_mgr_deinit(signal);

    /*
    THREADPOOL_HANDLE threadpool = threadpool_create(1);
    if (threadpool != NULL)
    {
        //for (size_t index = 0; index < 2; index++)
        {
            thread_num++;
            printf("Adding 3 work items\n");
            for (size_t inner = 0; inner < 4; inner++)
            {
                threadpool_initiate_work(threadpool, threadpool_start_func, &thread_num);
            }
            printf("Waiting for thread to be idle\n");
            //threadpool_wait_for_idle(threadpool);
            printf("Threadpool is idle\n");
        }
    thread_mgr_sleep(20000);

        //threadpool_wait_for_stop(threadpool);
        threadpool_destroy(threadpool);
    }
    else
    {
        printf("Failure creating threadpool\n");
    }*/

    printf("Press any key to exit\r\n");
    return 0;
}