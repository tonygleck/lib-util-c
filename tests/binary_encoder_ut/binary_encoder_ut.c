// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstddef>
#else
#include <stdlib.h>
#endif

#ifdef __cplusplus
#else
#include <stddef.h>
#endif

// Include the test tools.
#include "ctest.h"
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_charptr.h"

#define ENABLE_MOCKS
#include "umock_c/umock_c_prod.h"
#undef ENABLE_MOCKS

#include "lib-util-c/binary_encoder.h"

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

static const struct
{
    size_t input_len;
    const unsigned char* input_data;
    const char* base64_data;
} test_base64_value[] =
{
    {1,(const unsigned char*)"\xd4", "1A==" },
    {1,(const unsigned char*)"\xd5", "1Q==" },
    {1,(const unsigned char*)"\xd6", "1g==" },
    {1,(const unsigned char*)"\xd7", "1w==" },
    {1,(const unsigned char*)"\xd8", "2A==" },
    {1,(const unsigned char*)"\xd9", "2Q==" },
    {1,(const unsigned char*)"\xda", "2g==" },
    {1,(const unsigned char*)"\xdb", "2w==" },
    {2,(const unsigned char*)"\xdd\xee", "3e4=" },
    {2,(const unsigned char*)"\xdd\xff", "3f8=" },
    {2,(const unsigned char*)"\xee\x00", "7gA=" },
    {2,(const unsigned char*)"\xee\x11", "7hE=" },
    {2,(const unsigned char*)"\xee\x22", "7iI=" },
    {2,(const unsigned char*)"\xee\x33", "7jM=" },
    {2,(const unsigned char*)"\xee\x44", "7kQ=" },
    {3,(const unsigned char*)"\xeb\x8d\xbc", "6428" },
    {3,(const unsigned char*)"\xeb\x8d\xeb", "643r" },
    {3,(const unsigned char*)"\xeb\xbc\x00", "67wA" },
    {3,(const unsigned char*)"\xeb\xbc\x2f", "67wv" },
    {3,(const unsigned char*)"\xeb\xbc\x5e", "67xe" },
    {3,(const unsigned char*)"\xeb\xbc\x8d", "67yN" },
    {4,(const unsigned char*)"\xff\xff\x55\xff", "//9V/w==" },
    {4,(const unsigned char*)"\xff\xff\xaa\x00", "//+qAA==" },
    {4,(const unsigned char*)"\xff\xff\xaa\x55", "//+qVQ==" },
    {4,(const unsigned char*)"\xff\xff\xff\xff", "/////w==" },
    {5,(const unsigned char*)"\x00\xfc\xfc\xfc\x7e", "APz8/H4=" },
    {5,(const unsigned char*)"\xfc\xfc\x7e\x00\xfc", "/Px+APw=" },
    {5,(const unsigned char*)"\xfc\xfc\x7e\x7e\x00", "/Px+fgA=" },
    {6,(const unsigned char*)"\xa8\xa8\xa8\xa8\x00\xa8", "qKioqACo" },
    {6,(const unsigned char*)"\xa8\xa8\xa8\xa8\xa8\x00", "qKioqKgA" },
    {6,(const unsigned char*)"\xa8\xa8\xa8\xa8\xa8\xa8", "qKioqKio" },
    {7,(const unsigned char*)"\x00\x00\x00\x00\x00\x00\x00", "AAAAAAAAAA==" },
    {7,(const unsigned char*)"\x00\x00\x00\x00\x00\x00\xd3", "AAAAAAAA0w==" },
    {7,(const unsigned char*)"\x00\x00\x00\x00\x00\xd3\x00", "AAAAAADTAA==" },
    {8,(const unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00", "AAAAAAAAAAA=" },
    {9,(const unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00", "AAAAAAAAAAAA" },
    {10,(const unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", "AAAAAAAAAAAAAA==" }
};

static const struct
{
    size_t input_len;
    const unsigned char* input_data;
    const char* base32_data;
} test_base32_value[] =
{
    { 1, (const unsigned char*)"\x01", "ae======" },
    { 1, (const unsigned char*)"\x02", "ai======" },
    { 1, (const unsigned char*)"\x03", "am======" },
    { 1, (const unsigned char*)"\x04", "aq======" },
    { 1, (const unsigned char*)"\x05", "au======" },
    { 1, (const unsigned char*)"\x06", "ay======" },
    { 1, (const unsigned char*)"\x07", "a4======" },
    { 1, (const unsigned char*)"\x08", "ba======" },
    { 1, (const unsigned char*)"\x09", "be======" },
    { 1, (const unsigned char*)"\x0A", "bi======" },
    { 1, (const unsigned char*)"\x84", "qq======" },
    { 2, (const unsigned char*)"\x0b\x09", "bmeq====" },
    { 2, (const unsigned char*)"\x10\x20", "caqa====" },
    { 2, (const unsigned char*)"\x22\x99", "ekmq====" },
    { 2, (const unsigned char*)"\xFF\xFF", "777q====" },
    { 3, (const unsigned char*)"\x01\x10\x11", "aeibc===" },
    { 3, (const unsigned char*)"\x0A\x00\x0a", "biaau===" },
    { 3, (const unsigned char*)"\x99\xCC\xDD", "thgn2===" },
    { 4, (const unsigned char*)"\x00\x00\x00\x00", "aaaaaaa=" },
    { 4, (const unsigned char*)"\x01\x02\x03\x04", "aebagba=" },
    { 4, (const unsigned char*)"\xDD\xDD\xDD\xDD", "3xo53xi=" },
    { 5, (const unsigned char*)"\x01\x02\x03\x04\x05", "aebagbaf" },
    { 5, (const unsigned char*)"\x0a\x0b\x0c\x0d\x0e", "bifqydio" },
    { 6, (const unsigned char*)"\x66\x6f\x6f\x62\x61\x72", "mzxw6ytboi======" },
    { 6, (const unsigned char*)"\x0f\xff\x0e\xee\x0d\xdd", "b77q53qn3u======" },
    { 9, (const unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00", "aaaaaaaaaaaaaaa=" },
    { 9, (const unsigned char*)"\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", "a4eascqlbqgq4dy=" },

    { 10, (const unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", "aaaaaaaaaaaaaaaa" },
    { 10, (const unsigned char*)"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", "7777777777777777" },
    { 11, (const unsigned char*)"\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11", "a4eascqlbqgq4dyqce======" },
    { 11, (const unsigned char*)"\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99", "tgmztgmztgmztgmzte======" },
    { 14, (const unsigned char*)"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd", "3xo53xo53xo53xo53xo53xi=" },
    { 14, (const unsigned char*)"\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", "vkvkvkvkvkvkvkvkvkvkvkq=" },
    { 15, (const unsigned char*)"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", "aebagbafaydqqcikbmga2dqp" },
    { 16, (const unsigned char*)"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10", "aebagbafaydqqcikbmga2dqpca======" },
    { 17, (const unsigned char*)"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11", "aebagbafaydqqcikbmga2dqpcaiq====" },
    { 32, (const unsigned char*)"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20", "aebagbafaydqqcikbmga2dqpcaireeyuculbogazdinryhi6d4qa====" },
};

CTEST_BEGIN_TEST_SUITE(binary_encoder_ut)

    CTEST_SUITE_INITIALIZE()
    {
        (void)umock_c_init(on_umock_c_error);
    }

    CTEST_SUITE_CLEANUP()
    {
        umock_c_deinit();
    }

    CTEST_FUNCTION_INITIALIZE()
    {
        umock_c_reset_all_calls();
    }

    CTEST_FUNCTION_CLEANUP()
    {
    }

    CTEST_FUNCTION(bin_encoder_32_encode_source_NULL_fail)
    {
        //arrange
        char output[32];
        size_t output_len = 32;

        //act
        int result = bin_encoder_32_encode(NULL, 10, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, -1, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_encode_size_0_fail)
    {
        //arrange
        char output[32];
        const char* source = "";
        size_t output_len = 32;

        //act
        int result = bin_encoder_32_encode(source, 0, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, -1, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_encode_success)
    {
        //arrange
        char output[64];
        size_t num_elements = sizeof(test_base32_value)/sizeof(test_base32_value[0]);

        //act
        for (size_t index = 0; index < num_elements; index++)
        {
            size_t output_len = 64;
            int result = bin_encoder_32_encode(test_base32_value[index].input_data, test_base32_value[index].input_len, output, &output_len);

            //assert
            CTEST_ASSERT_ARE_EQUAL(int, 0, result, "bin_encoder_32_encode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(int, strlen(test_base32_value[index].base32_data), output_len, "bin_encoder_32_encode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(char_ptr, test_base32_value[index].base32_data, output, "bin_encoder_32_encode failure in test %lu", (unsigned long)index);

            //cleanup
        }
    }

    CTEST_FUNCTION(bin_encoder_32_encode_output_size_fail)
    {
        //arrange
        char output[64];
        size_t output_len = 2;
        size_t target_index = 30;

        //act5
        int result = bin_encoder_32_encode(test_base32_value[target_index].input_data, test_base32_value[target_index].input_len, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(int, strlen(test_base32_value[target_index].base32_data), output_len);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_decode_source_NULL_fail)
    {
        //arrange
        char output[32];
        size_t output_len = 64;

        //act
        int result = bin_encoder_32_decode(NULL, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_decode_output_NULL_fail)
    {
        //arrange
        const char* source = "aaaaaaaaaaaaaaaa";
        size_t output_len = 64;

        //act
        int result = bin_encoder_32_decode(source, NULL, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(size_t, 10, output_len);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_decode_success)
    {
        //arrange
        unsigned char output[64];
        size_t num_elements = sizeof(test_base32_value)/sizeof(test_base32_value[0]);

        //act
        for (size_t index = 0; index < num_elements; index++)
        {
            size_t output_len = 64;
            int result = bin_encoder_32_decode(test_base32_value[index].base32_data, output, &output_len);

            //assert
            CTEST_ASSERT_ARE_EQUAL(int, 0, result);
            CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(test_base32_value[index].input_data, output, output_len), "bin_encoder_32_decode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(int, test_base32_value[index].input_len, output_len, "bin_encoder_32_decode failure in test %lu", (unsigned long)index);

            //cleanup
        }
    }

    CTEST_FUNCTION(bin_encoder_32_decode_partial_source_NULL_fail)
    {
        //arrange
        char output[32];
        size_t output_len = 64;

        //act
        int result = bin_encoder_32_decode_partial(NULL, 32, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_decode_partial_output_NULL_fail)
    {
        //arrange
        const char* source = "aaaaaaaaaaaaaaaa_DO_NOT_ENCODE";
        size_t output_len = 64;

        //act
        int result = bin_encoder_32_decode_partial(source, 17, NULL, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(size_t, 10, output_len);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_decode_partial_string_success)
    {
        //arrange
        const char* source = "AAAAAAAAAAAAAA==_DO_NOT_ENCODE";
        const unsigned char src_encoded[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        unsigned char output[64];
        size_t output_len = 64;

        //act
        int result = bin_encoder_32_decode_partial(source, 16, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(src_encoded, output, output_len), "bin_encoder_32_decode_partial failure in test");
        CTEST_ASSERT_ARE_EQUAL(size_t, 9, output_len);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_32_decode_partial_success)
    {
        //arrange
        unsigned char output[64];
        size_t num_elements = sizeof(test_base32_value)/sizeof(test_base32_value[0]);

        //act
        for (size_t index = 0; index < num_elements; index++)
        {
            size_t output_len = 64;
            size_t data_size = strlen(test_base32_value[index].base32_data);
            int result = bin_encoder_32_decode_partial(test_base32_value[index].base32_data, data_size, output, &output_len);

            //assert
            CTEST_ASSERT_ARE_EQUAL(int, 0, result);
            CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(test_base32_value[index].input_data, output, output_len), "bin_encoder_32_decode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(int, test_base32_value[index].input_len, output_len, "bin_encoder_32_decode failure in test %lu", (unsigned long)index);

            //cleanup
        }
    }

    CTEST_FUNCTION(bin_encoder_64_encode_source_NULL_fail)
    {
        //arrange
        char output[32];
        size_t output_len = 32;

        //act
        int result = bin_encoder_64_encode(NULL, 10, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, -1, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_64_encode_size_0_fail)
    {
        //arrange
        char output[32];
        const char* source = "";
        size_t output_len = 32;

        //act
        int result = bin_encoder_64_encode(source, 0, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, -1, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_64_encode_success)
    {
        //arrange
        char output[64];
        size_t num_elements = sizeof(test_base64_value)/sizeof(test_base64_value[0]);

        //act
        for (size_t index = 0; index < num_elements; index++)
        {
            size_t output_len = 64;
            int result = bin_encoder_64_encode(test_base64_value[index].input_data, test_base64_value[index].input_len, output, &output_len);

            //assert
            CTEST_ASSERT_ARE_EQUAL(int, 0, result, "bin_encoder_64_encode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(int, strlen(test_base64_value[index].base64_data), output_len, "bin_encoder_64_encode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(char_ptr, test_base64_value[index].base64_data, output, "bin_encoder_64_encode failure in test %lu", (unsigned long)index);

            //cleanup
        }
    }

    CTEST_FUNCTION(bin_encoder_64_decode_source_NULL_fail)
    {
        //arrange
        char output[32];
        size_t output_len = 64;

        //act
        int result = bin_encoder_64_decode(NULL, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_64_decode_output_NULL_fail)
    {
        //arrange
        const char* source = "AAAAAAAAAA==";
        size_t output_len = 64;

        //act
        int result = bin_encoder_64_decode(source, NULL, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(size_t, 7, output_len);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_64_decode_success)
    {
        //arrange
        unsigned char output[64];
        size_t num_elements = sizeof(test_base64_value)/sizeof(test_base64_value[0]);

        //act
        for (size_t index = 0; index < num_elements; index++)
        {
            size_t output_len = 64;
            int result = bin_encoder_64_decode(test_base64_value[index].base64_data, output, &output_len);

            //assert
            CTEST_ASSERT_ARE_EQUAL(int, 0, result);
            CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(test_base64_value[index].input_data, output, output_len), "bin_encoder_64_decode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(int, test_base64_value[index].input_len, output_len, "bin_encoder_64_decode failure in test %lu", (unsigned long)index);

            //cleanup
        }
    }

    CTEST_FUNCTION(bin_encoder_64_decode_partial_source_NULL_fail)
    {
        //arrange
        char output[32];
        size_t output_len = 32;

        //act
        int result = bin_encoder_64_decode_partial(NULL, 0, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_64_decode_partial_output_NULL_fail)
    {
        //arrange
        const char* source = "AAAAAAAAAA==";
        size_t output_len = 64;

        //act
        int result = bin_encoder_64_decode_partial(source, strlen(source), NULL, &output_len);

        //assert
        CTEST_ASSERT_ARE_NOT_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(size_t, 7, output_len);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_64_decode_partial_string_success)
    {
        //arrange
        const char* source = "AAAAAAAAAA==_DO_NOT_ENCODE";
        char output[64];
        size_t output_len = 64;

        //act
        int result = bin_encoder_64_decode_partial(source, 12, output, &output_len);

        //assert
        CTEST_ASSERT_ARE_EQUAL(int, 0, result);
        CTEST_ASSERT_ARE_EQUAL(size_t, 7, output_len);

        //cleanup
    }

    CTEST_FUNCTION(bin_encoder_64_decode_partial_success)
    {
        //arrange
        unsigned char output[64];
        size_t num_elements = sizeof(test_base64_value)/sizeof(test_base64_value[0]);

        //act
        for (size_t index = 0; index < num_elements; index++)
        {
            size_t output_len = 64;
            int result = bin_encoder_64_decode_partial(test_base64_value[index].base64_data, strlen(test_base64_value[index].base64_data), output, &output_len);

            //assert
            CTEST_ASSERT_ARE_EQUAL(int, 0, result);
            CTEST_ASSERT_ARE_EQUAL(int, 0, memcmp(test_base64_value[index].input_data, output, output_len), "bin_encoder_64_decode failure in test %lu", (unsigned long)index);
            CTEST_ASSERT_ARE_EQUAL(int, test_base64_value[index].input_len, output_len, "bin_encoder_64_decode failure in test %lu", (unsigned long)index);

            //cleanup
        }
    }

CTEST_END_TEST_SUITE(binary_encoder_ut)
