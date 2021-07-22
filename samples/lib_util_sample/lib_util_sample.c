
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
#include "lib-util-c/dllist.h"
#include "lib-util-c/threadpool.h"

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

typedef struct TEST_LIST_ITEM_TAG
{
    DLLIST_ENTRY dlist_entry;
    const char* test_string;
    size_t test_value;
    size_t offset;
} TEST_LIST_ITEM;

static void test_dlist_items(void)
{
    DLLIST_ENTRY list_items;

    dllist_init_list_head(&list_items);

    TEST_LIST_ITEM test_items_1 = {0};
    TEST_LIST_ITEM test_items_2 = {0};
    TEST_LIST_ITEM test_items_3 = {0};
    TEST_LIST_ITEM test_items_4 = {0};
    TEST_LIST_ITEM test_items_5 = {0};

    test_items_1.test_string = "test_string_1";
    test_items_1.test_value = 1;
    dllist_insert_tail(&list_items, &test_items_1.dlist_entry);

    test_items_2.test_string = "test_string_2";
    test_items_2.test_value = 2;
    dllist_insert_tail(&list_items, &test_items_2.dlist_entry);

    test_items_3.test_string = "test_string_3";
    test_items_3.test_value = 3;
    dllist_insert_tail(&list_items, &test_items_3.dlist_entry);

    test_items_4.test_string = "test_string_4";
    test_items_4.test_value = 4;
    dllist_insert_tail(&list_items, &test_items_4.dlist_entry);

    test_items_5.test_string = "test_string_5";
    test_items_5.test_value = 5;
    dllist_insert_tail(&list_items, &test_items_5.dlist_entry);

    size_t bytes_required = 100;
    do
    {
        /* code */
    } while (bytes_required != 0);


    PDLLIST_ENTRY list_entry = list_items.fwd_link;
    while (list_entry != &list_items)
    {
        TEST_LIST_ITEM* list_item = CONTAINING_RECORD(list_entry, TEST_LIST_ITEM, dlist_entry);
        printf("Item %s\n", list_item->test_string);

        list_entry = dllist_remove_head(list_entry);
        //list_entry->fwd_link;
    }
    printf("Should be empty %s", dllist_is_empty(&list_items) == 0 ? "true" : "false" );
}

static const char* test_string[] =
{
    "string 0",
    "string 1",
    "string 2",
    "string 3",
    "string 4",
    "string 5"
};

volatile uint32_t g_thread_cntr;
static void threadpool_work_routine(void* arg)
{
    const char* incoming_string = (const char*)arg;


}

static void show_threadpool_sample(void)
{
    size_t threadpool_size = sizeof(test_string)/sizeof(test_string[0]);
    THREADPOOL_HANDLE threadpool_handle = threadpool_create(threadpool_size);
    if (threadpool_handle == NULL)
    {
        printf("Failure creating threadpool");
    }
    else
    {

        for (size_t index = 0; index < threadpool_size; index++)
        {
            threadpool_add_item(threadpool_handle, threadpool_work_routine, (void*)test_string[index]);
        }

        threadpool_destroy(threadpool_handle);
    }
}

int main()
{
    printf("Starting\r\n");

    //test_dlist_items();

    show_threadpool_sample();

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