
#include "stdio.h"
#include "lib-util-c/item_map.h"

#define START_HASH_VALUE    5381

static uint32_t default_hash_function(const char* key)
{
    uint32_t result = START_HASH_VALUE;
    char str_value;

    while (str_value = *key++)
    {
        result = ((result << 5) + result) + str_value; // result * 33 + str_value;
    }
    return result;
}

static void construct_item_map(void)
{
    ITEM_MAP_HANDLE item_map;
    if ((item_map = item_map_create(0, NULL, NULL, NULL)) == NULL)
    {
        printf("Failure creating item map client\r\n");
    }
    else
    {
        int value = 12;
        item_map_add_item(item_map, "value_1", &value, sizeof(int));
        value = 22;
        item_map_add_item(item_map, "value_2", &value, sizeof(int));
        value = 32;
        item_map_add_item(item_map, "value_3", &value, sizeof(int));

        const int* test_item = item_map_get_item(item_map, "value_2");

        item_map_destroy(item_map);
    }
}

int main()
{
    uint32_t value1 = default_hash_function("aaaaa") % 10;
    uint32_t value2 = default_hash_function("aaaaaa") % 10;
    uint32_t value3 = default_hash_function("aaaba") % 10;
    uint32_t value4 = default_hash_function("aabaa") % 10;
    uint32_t value5 = default_hash_function("abaaa") % 10;
    uint32_t value6 = default_hash_function("baaaa") % 10;
    uint32_t value7 = default_hash_function("bbbbb") % 10;
    uint32_t value8 = default_hash_function("bbbba") % 10;
    construct_item_map();

    printf("Press any key to exit\r\n");
    return 0;
}