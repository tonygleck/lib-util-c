
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
static int i = 0;

#include "lib-util-c/item_map.h"
#include "lib-util-c/thread_mgr.h"
#include "lib-util-c/buffer_alloc.h"
#include "lib-util-c/sha_algorithms.h"

#define START_HASH_VALUE    5381

static int signal_recv_count = 0;
static timer_t timerid;

void sigalrm_handler(int signum)
{
  printf("SIGALRM received, count :%d\n", signal_recv_count);
  signal_recv_count++;
}

void sighler (union sigval val)
{
    printf("Handler entered with value :%d for %d times\n", val.sival_int, ++i);

    if (i == 20)
    {
        //delete the timer.
        timer_delete(timerid);
    }
}

typedef struct CLIENT_E2E_DATA_TAG
{
    BYTE_BUFFER sent_data;
} CLIENT_E2E_DATA;

int main()
{
    printf("Starting\r\n");
    unsigned char buffer[128];
    for (size_t index = 0; index < 128; index++)
    {
        buffer[index] = 0x10 + index;
    }

    CLIENT_E2E_DATA e2e_data = {0};
    for (size_t index = 0; index < 10; index++)
    {
        byte_buffer_construct(&e2e_data.sent_data, buffer, 128);
    printf("payload size %ld\n", e2e_data.sent_data.payload_size);
    }
    byte_buffer_free(&e2e_data.sent_data);

    // struct sigevent sig;
    // sig.sigev_notify = SIGEV_THREAD;
    // sig.sigev_notify_function = sighler;
    // sig.sigev_value.sival_int = 20;
    // sig.sigev_notify_attributes = NULL;//&attr;

    // //create a new timer.
    // int ret = timer_create(CLOCK_REALTIME, &sig, &timerid);
    // if (ret == 0)
    // {
    //     struct itimerspec in, out;
    //     in.it_value.tv_sec = 1;
    //     in.it_value.tv_nsec = 0;
    //     in.it_interval.tv_sec = 1;
    //     in.it_interval.tv_nsec = 0;
    //     //issue the periodic timer request here.
    //     ret = timer_settime(timerid, 0, &in, &out);
    //     if(ret == 0)
    //         sleep(2);
    //     else
    //         printf("timer_settime() failed with %d\n", errno);
    // }
    // else
    // {    unsigned char buffer[128];


    // char a[200];
    // read(2, &a, 199);
    printf("Press any key to exit\r\n");
    return 0;
}