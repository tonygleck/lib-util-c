// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "lib-util-c/binary_encoder.h"
#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"

static const char BASE32_VALUES[] = "abcdefghijklmnopqrstuvwxyz234567=";
static const unsigned char BASE32_EQUAL_SIGN = 32;

#define splitInt(intVal, bytePos)   (char)((intVal >> (bytePos << 3)) & 0xFF)
#define joinChars(a, b, c, d) (uint32_t)((uint32_t)a + ((uint32_t)b << 8) + ((uint32_t)c << 16) + ((uint32_t)d << 24))
#define TARGET_BLOCK_SIZE       5
#define BASE32_INPUT_SIZE       8
#define ASCII_VALUE_MAX         0x80
#define INVALID_CHAR_POS        260

typedef enum ENCODING_TYPE_TAG
{
    TYPE_BASE_64,
    TYPE_BASE_32
} ENCODING_TYPE;

static char base_64_encode_char(unsigned char val)
{
    char result;
    if (val < 26)
    {
        result = 'A' + (char)val;
    }
    else if (val < 52)
    {
        result = 'a' + ((char)val - 26);
    }
    else if (val < 62)
    {
        result = '0' + ((char)val - 52);
    }
    else if (val == 62)
    {
        result = '+';
    }
    else
    {
        result = '/';
    }
    return result;
}

static char base_64_base_16(unsigned char val)
{
    const uint32_t base64b16values[4] = {
        joinChars('A', 'E', 'I', 'M'),
        joinChars('Q', 'U', 'Y', 'c'),
        joinChars('g', 'k', 'o', 's'),
        joinChars('w', '0', '4', '8')
    };
    return splitInt(base64b16values[val >> 2], (val & 0x03));
}

static char base_64_base_8(unsigned char val)
{
    const uint32_t base64b8values = joinChars('A', 'Q', 'g', 'w');
    return splitInt(base64b8values, val);
}

static unsigned char convert_value_to_base32_char(unsigned char value)
{
    unsigned char result;
    if (value >= 50 && value <= 55)
    {
        result = 0x1a+(value-50);
    }
    else if (value == 61)
    {
        result = 0x20;
    }
    else if ((value <= 49) || (value >= 56 && value <= 64))
    {
        result = 0xFF;
    }
    else if (value >= 65 && value <= 90)
    {
        result = 0x00 + (value - 65);
    }
    else if (value >= 91 && value <= 96)
    {
        result = 0xFF;
    }
    else if (value >= 97 && value <= 123)
    {
        result = 0x00 + (value - 97);
    }
    else // value > 123
    {
        result = 0xFF;
    }
    return result;
}

static size_t calculate_encoding_len(size_t encode_len, ENCODING_TYPE type)
{
    size_t result;
    if (type == TYPE_BASE_64)
    {
        result = (encode_len == 0) ? (0) : ((((encode_len - 1) / 3) + 1) * 4);
    }
    else
    {
        result = (((encode_len + TARGET_BLOCK_SIZE - 1) / TARGET_BLOCK_SIZE) * 8);
    }
    return result;
}

static size_t calculate_decoding_len(const char* source, size_t decode_len, ENCODING_TYPE type)
{
    size_t result;
    if (type == TYPE_BASE_64)
    {
        result = decode_len / 4 * 3;
        if (source[decode_len - 1] == '=')
        {
            if (source[decode_len - 2] == '=')
            {
                result --;
            }
            result--;
        }
    }
    else
    {
        result = ((decode_len*TARGET_BLOCK_SIZE) / 8);
    }
    return result;
}

static int base64_to_value(char base64_char, unsigned char* value)
{
    int result = 0;
    if (('A' <= base64_char) && (base64_char <= 'Z'))
    {
        *value = base64_char - 'A';
    }
    else if (('a' <= base64_char) && (base64_char <= 'z'))
    {
        *value = ('Z' - 'A') + 1 + (base64_char - 'a');
    }
    else if (('0' <= base64_char) && (base64_char <= '9'))
    {
        *value = ('Z' - 'A') + 1 + ('z' - 'a') + 1 + (base64_char - '0');
    }
    else if ('+' == base64_char)
    {
        *value = 62;
    }
    else if ('/' == base64_char)
    {
        *value = 63;
    }
    else
    {
        *value = 0;
        result = -1;
    }
    return result;
}

static size_t get_base64_char_count(const char* source)
{
    size_t length = 0;
    unsigned char ignore;
    while (base64_to_value(source[length], &ignore) != -1)
    {
        length++;
    }
    return length;
}

static int decode_base32_source_value(const char* source, size_t src_len, unsigned char* output, size_t* result_len)
{
    int result;
    //size_t test = (src_len % BASE32_INPUT_SIZE);
    if (src_len % BASE32_INPUT_SIZE != 0)
    {
        log_error("Failure invalid input length %lu", (unsigned long)src_len);
        result = -1;
    }
    else
    {
        size_t allocation_len = calculate_decoding_len(source, src_len, TYPE_BASE_32);
        if (*result_len < allocation_len)
        {
            log_error("Failure Output len is less than needed length %lu", (unsigned long)allocation_len);
            *result_len = allocation_len;
            result = -1;
        }
        else
        {
            size_t dest_size = 0;
            unsigned char input[8];
            bool continue_processing = true;
            unsigned char* dest_buff = output;

            const char* iterator = source;
            while (*iterator != '\0')
            {
                for (size_t index = 0; index < BASE32_INPUT_SIZE; index++)
                {
                    input[index] = *iterator;
                    iterator++;
                    if (input[index] >= ASCII_VALUE_MAX)
                    {
                        log_error("Failure source encoding");
                        continue_processing = false;
                        *result_len = allocation_len;
                        result = -1;
                        break;
                    }
                    input[index] = convert_value_to_base32_char(input[index]);
                }
                if (!continue_processing)
                {
                    *result_len = allocation_len;
                    result = -1;
                    break;
                }
                else if ((dest_size + TARGET_BLOCK_SIZE) > allocation_len)
                {
                    log_error("Failure target length exceeded");
                    *result_len = allocation_len;
                    result = -1;
                    continue_processing = false;
                    break;
                }
                else
                {
                    // Codes_SRS_BASE32_07_025: [ base32_decode_impl shall group 5 bytes at a time into the temp buffer. ]
                    *dest_buff++ = ((input[0] & 0x1f) << 3) | ((input[1] & 0x1c) >> 2);
                    *dest_buff++ = ((input[1] & 0x03) << 6) | ((input[2] & 0x1f) << 1) | ((input[3] & 0x10) >> 4);
                    *dest_buff++ = ((input[3] & 0x0f) << 4) | ((input[4] & 0x1e) >> 1);
                    *dest_buff++ = ((input[4] & 0x01) << 7) | ((input[5] & 0x1f) << 2) | ((input[6] & 0x18) >> 3);
                    *dest_buff++ = ((input[6] & 0x07) << 5) | (input[7] & 0x1f);
                    dest_size += TARGET_BLOCK_SIZE;
                    // If there is padding remove it
                    // Because we are packing 5 bytes into an 8 byte variable we need to check every other
                    // variable for padding
                    if (input[7] == BASE32_EQUAL_SIGN)
                    {
                        --dest_size;
                        if (input[5] == BASE32_EQUAL_SIGN)
                        {
                            --dest_size;
                            if (input[4] == BASE32_EQUAL_SIGN)
                            {
                                --dest_size;
                                if (input[2] == BASE32_EQUAL_SIGN)
                                {
                                    --dest_size;
                                }
                            }
                        }
                    }
                    *result_len = dest_size;
                    result = 0;
                }
            }
        }
    }
    return result;
}

static void decode_base64_source_value(const char* source, unsigned char* output)
{
    size_t index_first_encoded_char = 0;
    size_t decoded_index = 0;
    size_t num_of_encoded_char = get_base64_char_count(source);
    unsigned char c1;
    unsigned char c2;
    unsigned char c3;
    while (num_of_encoded_char >= 4)
    {
        unsigned char c4;
        (void)base64_to_value(source[index_first_encoded_char], &c1);
        (void)base64_to_value(source[index_first_encoded_char + 1], &c2);
        (void)base64_to_value(source[index_first_encoded_char + 2], &c3);
        (void)base64_to_value(source[index_first_encoded_char + 3], &c4);
        output[decoded_index++] = (c1 << 2) | (c2 >> 4);
        output[decoded_index++] = ((c2 & 0x0f) << 4) | (c3 >> 2);
        output[decoded_index++] = ((c3 & 0x03) << 6) | c4;
        num_of_encoded_char -= 4;
        index_first_encoded_char += 4;
    }
    if (num_of_encoded_char == 2)
    {
        (void)base64_to_value(source[index_first_encoded_char], &c1);
        (void)base64_to_value(source[index_first_encoded_char + 1], &c2);
        output[decoded_index] = (c1 << 2) | (c2 >> 4);
    }
    else if (num_of_encoded_char == 3)
    {
        (void)base64_to_value(source[index_first_encoded_char], &c1);
        (void)base64_to_value(source[index_first_encoded_char + 1], &c2);
        (void)base64_to_value(source[index_first_encoded_char + 2], &c3);
        output[decoded_index++] = (c1 << 2) | (c2 >> 4);
        output[decoded_index] = ((c2 & 0x0f) << 4) | (c3 >> 2);
    }
}

#if 0
/*returns the count of original bytes before being base64 encoded*/
/*notice NO validation of the content of encodedString. Its length is validated to be a multiple of 4.*/
static size_t Base64decode_len(const char *encodedString)
{
    size_t result;
    size_t sourceLength = strlen(encodedString);

    if (sourceLength == 0)
    {
        result = 0;
    }
    else
    {
        result = sourceLength / 4 * 3;
        if (encodedString[sourceLength - 1] == '=')
        {
            if (encodedString[sourceLength - 2] == '=')
            {
                result --;
            }
            result--;
        }
    }
    return result;
}

static void Base64decode(unsigned char *decodedString, const char *base64String)
{

    size_t numberOfEncodedChars;
    size_t indexOfFirstEncodedChar;
    size_t decodedIndex;

    //
    // We can only operate on individual bytes.  If we attempt to work
    // on anything larger we could get an alignment fault on some
    // architectures
    //

    numberOfEncodedChars = numberOfBase64Characters(base64String);
    indexOfFirstEncodedChar = 0;
    decodedIndex = 0;
    while (numberOfEncodedChars >= 4)
    {
        unsigned char c1;
        unsigned char c2;
        unsigned char c3;
        unsigned char c4;
        (void)base64toValue(base64String[indexOfFirstEncodedChar], &c1);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 1], &c2);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 2], &c3);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 3], &c4);
        decodedString[decodedIndex] = (c1 << 2) | (c2 >> 4);
        decodedIndex++;
        decodedString[decodedIndex] = ((c2 & 0x0f) << 4) | (c3 >> 2);
        decodedIndex++;
        decodedString[decodedIndex] = ((c3 & 0x03) << 6) | c4;
        decodedIndex++;
        numberOfEncodedChars -= 4;
        indexOfFirstEncodedChar += 4;

    }

    if (numberOfEncodedChars == 2)
    {
        unsigned char c1;
        unsigned char c2;
        (void)base64toValue(base64String[indexOfFirstEncodedChar], &c1);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 1], &c2);
        decodedString[decodedIndex] = (c1 << 2) | (c2 >> 4);
    }
    else if (numberOfEncodedChars == 3)
    {
        unsigned char c1;
        unsigned char c2;
        unsigned char c3;
        (void)base64toValue(base64String[indexOfFirstEncodedChar], &c1);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 1], &c2);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 2], &c3);
        decodedString[decodedIndex] = (c1 << 2) | (c2 >> 4);
        decodedIndex++;
        decodedString[decodedIndex] = ((c2 & 0x0f) << 4) | (c3 >> 2);
    }
}

BUFFER_HANDLE Base64_Decoder(const char* source)
{
    BUFFER_HANDLE result;
    /*Codes_SRS_BASE64_06_008: [If source is NULL then Base64_Decode shall return NULL.]*/
    if (source == NULL)
    {
        LogError("invalid parameter const char* source=%p", source);
        result = NULL;
    }
    else
    {
        if ((strlen(source) % 4) != 0)
        {
            /*Codes_SRS_BASE64_06_011: [If the source string has an invalid length for a base 64 encoded string then Base64_Decode shall return NULL.]*/
            LogError("Invalid length Base64 string!");
            result = NULL;
        }
        else
        {
            if ((result = BUFFER_new()) == NULL)
            {
                /*Codes_SRS_BASE64_06_010: [If there is any memory allocation failure during the decode then Base64_Decode shall return NULL.]*/
                LogError("Could not create a buffer to decoding.");
            }
            else
            {
                size_t sizeOfOutputBuffer = Base64decode_len(source);
                /*Codes_SRS_BASE64_06_009: [If the string pointed to by source is zero length then the handle returned shall refer to a zero length buffer.]*/
                if (sizeOfOutputBuffer > 0)
                {
                    if (BUFFER_pre_build(result, sizeOfOutputBuffer) != 0)
                    {
                        /*Codes_SRS_BASE64_06_010: [If there is any memory allocation failure during the decode then Base64_Decode shall return NULL.]*/
                        LogError("Could not prebuild a buffer for base 64 decoding.");
                        BUFFER_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        Base64decode(BUFFER_u_char(result), source);
                    }
                }
            }
        }
    }
    return result;
}


static STRING_HANDLE Base64_Encode_Internal(const unsigned char* source, size_t size)
{
    STRING_HANDLE result;
    size_t neededSize = 0;
    char* encoded;
    size_t currentPosition = 0;
    neededSize += (size == 0) ? (0) : ((((size - 1) / 3) + 1) * 4);
    neededSize += 1; /*+1 because \0 at the end of the string*/
    /*Codes_SRS_BASE64_06_006: [If when allocating memory to produce the encoding a failure occurs then Base64_Encoder shall return NULL.]*/
    encoded = (char*)malloc(neededSize);
    if (encoded == NULL)
    {
        result = NULL;
        LogError("Base64_Encoder:: Allocation failed.");
    }
    else
    {
        /*b0            b1(+1)          b2(+2)
        7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
        |----c1---| |----c2---| |----c3---| |----c4---|
        */

        size_t destinationPosition = 0;
        while (size - currentPosition >= 3)
        {
            char c1 = base64char(source[currentPosition] >> 2);
            char c2 = base64char(
                ((source[currentPosition] & 3) << 4) |
                    (source[currentPosition + 1] >> 4)
            );
            char c3 = base64char(
                ((source[currentPosition + 1] & 0x0F) << 2) |
                    ((source[currentPosition + 2] >> 6) & 3)
            );
            char c4 = base64char(
                source[currentPosition + 2] & 0x3F
            );
            currentPosition += 3;
            encoded[destinationPosition++] = c1;
            encoded[destinationPosition++] = c2;
            encoded[destinationPosition++] = c3;
            encoded[destinationPosition++] = c4;

        }
        if (size - currentPosition == 2)
        {
            char c1 = base64char(source[currentPosition] >> 2);
            char c2 = base64char(
                ((source[currentPosition] & 0x03) << 4) |
                    (source[currentPosition + 1] >> 4)
            );
            char c3 = base64b16(source[currentPosition + 1] & 0x0F);
            encoded[destinationPosition++] = c1;
            encoded[destinationPosition++] = c2;
            encoded[destinationPosition++] = c3;
            encoded[destinationPosition++] = '=';
        }
        else if (size - currentPosition == 1)
        {
            char c1 = base64char(source[currentPosition] >> 2);
            char c2 = base64b8(source[currentPosition] & 0x03);
            encoded[destinationPosition++] = c1;
            encoded[destinationPosition++] = c2;
            encoded[destinationPosition++] = '=';
            encoded[destinationPosition++] = '=';
        }

        /*null terminating the string*/
        encoded[destinationPosition] = '\0';
        /*Codes_SRS_BASE64_06_007: [Otherwise Base64_Encoder shall return a pointer to STRING, that string contains the base 64 encoding of input.]*/
        result = STRING_new_with_memory(encoded);
        if (result == NULL)
        {
            free(encoded);
            LogError("Base64_Encoder:: Allocation failed for return value.");
        }
    }
    return result;
}
#endif

int bin_encoder_32_encode(const unsigned char* source, size_t src_size, char* output, size_t* result_len)
{
    int result;
    if (source == NULL || src_size == 0)
    {
        log_error("Invalid parameter specified");
        result = -1;
    }
    else
    {
        size_t output_len = calculate_encoding_len(src_size, TYPE_BASE_32);
        if (output == NULL || *result_len <= output_len)
        {
            log_error("Buffer size is insufficient");
            *result_len = output_len;
            result = -1;
        }
        else
        {
            const unsigned char* iterator = source;
            size_t block_len = 0;
            size_t res_index = 0;
            unsigned char pos1 = 0;
            unsigned char pos2 = 0;
            unsigned char pos3 = 0;
            unsigned char pos4 = 0;
            unsigned char pos5 = 0;
            unsigned char pos6 = 0;
            unsigned char pos7 = 0;
            unsigned char pos8 = 0;

            memset(output, 0, output_len + 1);
            while (src_size >= 1 && output != NULL)
            {
                pos1 = pos2 = pos3 = pos4 = pos5 = pos6 = pos7 = pos8 = 0;
                block_len = src_size > TARGET_BLOCK_SIZE ? TARGET_BLOCK_SIZE : src_size;
                // Fall through switch block to process the 5 (or smaller) block
                switch (block_len)
                {
                case 5:
                    pos8 = (iterator[4] & 0x1f);
                    pos7 = ((iterator[4] & 0xe0) >> 5);
                    // fall through
                case 4:
                    pos7 |= ((iterator[3] & 0x03) << 3);
                    pos6 = ((iterator[3] & 0x7c) >> 2);
                    pos5 = ((iterator[3] & 0x80) >> 7);
                    // fall through
                case 3:
                    pos5 |= ((iterator[2] & 0x0f) << 1);
                    pos4 = ((iterator[2] & 0xf0) >> 4);
                    // fall through
                case 2:
                    pos4 |= ((iterator[1] & 0x01) << 4);
                    pos3 = ((iterator[1] & 0x3e) >> 1);
                    pos2 = ((iterator[1] & 0xc0) >> 6);
                    // fall through
                case 1:
                    pos2 |= ((iterator[0] & 0x07) << 2);
                    pos1 = ((iterator[0] & 0xf8) >> 3);
                    break;
                }
                // Move the iterator the block size
                iterator += block_len;
                // and decrement the src_size;
                src_size -= block_len;

                /* Codes_SRS_BASE32_07_012: [ If the src_size is not divisible by 8, base32_encode_impl shall pad the remaining places with =. ] */
                switch (block_len)
                {
                    case 1: pos3 = pos4 = 32; // fall through
                    case 2: pos5 = 32; // fall through
                    case 3: pos6 = pos7 = 32; // fall through
                    case 4: pos8 = 32; // fall through
                    case 5:
                        break;
                }

                /* Codes_SRS_BASE32_07_011: [ base32_encode_impl shall then map the 5 bit chunks into one of the BASE32 values (a-z,2,3,4,5,6,7) values. ] */
                output[res_index++] = BASE32_VALUES[pos1];
                output[res_index++] = BASE32_VALUES[pos2];
                output[res_index++] = BASE32_VALUES[pos3];
                output[res_index++] = BASE32_VALUES[pos4];
                output[res_index++] = BASE32_VALUES[pos5];
                output[res_index++] = BASE32_VALUES[pos6];
                output[res_index++] = BASE32_VALUES[pos7];
                output[res_index++] = BASE32_VALUES[pos8];
            }
            *result_len = output_len;
            result = 0;
        }
    }
    return result;
}

int bin_encoder_32_decode(const char* source, unsigned char* output, size_t* result_len)
{
    int result;
    if (source == NULL || result_len == NULL)
    {
        log_error("Invalid parameter specified");
        result = -1;
    }
    else if (output == NULL)
    {
        *result_len = calculate_decoding_len(source, strlen(source), TYPE_BASE_32);
        log_error("output parameter value NULL");
        result = -1;
    }
    else
    {
        size_t src_len = strlen(source);
        result = decode_base32_source_value(source, src_len, output, result_len);
    }
    return result;
}

int bin_encoder_32_decode_partial(const char* source, size_t src_len, unsigned char* output, size_t* result_len)
{
    int result;
    if (source == NULL || src_len == 0 || result_len == NULL)
    {
        log_error("Invalid parameter specified");
        result = -1;
    }
    else if (output == NULL)
    {
        *result_len = calculate_decoding_len(source, src_len, TYPE_BASE_32);
        log_error("output parameter value NULL");
        result = -1;
    }
    else
    {
        result = decode_base32_source_value(source, src_len, output, result_len);
    }
    return result;
}

int bin_encoder_64_encode(const unsigned char* source, size_t src_len, char* output, size_t* result_len)
{
    int result;
    if (source == NULL || src_len == 0 || result_len == NULL)
    {
        log_error("Invalid parameter specified");
        result = -1;
    }
    else if (output == NULL)
    {
        *result_len = calculate_encoding_len(src_len, TYPE_BASE_64);
        result = -1;
    }
    else
    {
        size_t orig_len = *result_len;
        *result_len = calculate_encoding_len(src_len, TYPE_BASE_64);
        if (orig_len > *result_len)
        {
            size_t curr_pos = 0;
            size_t dest_pos = 0;
            while (src_len - curr_pos >= 3)
            {
                char c1 = base_64_encode_char(source[curr_pos] >> 2);
                char c2 = base_64_encode_char(((source[curr_pos] & 3) << 4) |
                    (source[curr_pos + 1] >> 4) );
                char c3 = base_64_encode_char(((source[curr_pos + 1] & 0x0F) << 2) |
                    ((source[curr_pos + 2] >> 6) & 3));
                char c4 = base_64_encode_char(source[curr_pos + 2] & 0x3F);
                curr_pos += 3;
                output[dest_pos++] = c1;
                output[dest_pos++] = c2;
                output[dest_pos++] = c3;
                output[dest_pos++] = c4;
            }
            if (src_len - curr_pos == 2)
            {
                char c1 = base_64_encode_char(source[curr_pos] >> 2);
                char c2 = base_64_encode_char(((source[curr_pos] & 0x03) << 4) |
                    (source[curr_pos + 1] >> 4) );
                char c3 = base_64_base_16(source[curr_pos + 1] & 0x0F);
                output[dest_pos++] = c1;
                output[dest_pos++] = c2;
                output[dest_pos++] = c3;
                output[dest_pos++] = '=';
            }
            else if (src_len - curr_pos == 1)
            {
                char c1 = base_64_encode_char(source[curr_pos] >> 2);
                char c2 = base_64_base_8(source[curr_pos] & 0x03);
                output[dest_pos++] = c1;
                output[dest_pos++] = c2;
                output[dest_pos++] = '=';
                output[dest_pos++] = '=';
            }
            output[dest_pos++] = '\0';
        }
        else
        {
            log_error("Invalid length specified need %lu bytes", (unsigned long)*result_len);
        }
        result = 0;
    }
    return result;
}

int bin_encoder_64_decode(const char* source, unsigned char* output, size_t* result_len)
{
    int result;
    if (source == NULL)
    {
        log_error("Invalid parameter specified");
        result = -1;
    }
    else if (output == NULL)
    {
        size_t src_len = strlen(source);
        *result_len = calculate_decoding_len(source, src_len, TYPE_BASE_64);
        result = -1;
    }
    else
    {
        size_t decode_len;
        size_t src_len = strlen(source);
        if ((src_len % 4) != 0)
        {
            log_error("Invalid length of base64 source");
            result = -1;
        }
        else if (*result_len < (decode_len = calculate_decoding_len(source, src_len, TYPE_BASE_64)))
        {
            *result_len = decode_len;
            log_error("Invalid length of base64 source");
            result = -1;
        }
        else
        {
            *result_len = decode_len;
            decode_base64_source_value(source, output);
            result = 0;
        }
    }
    return result;
}

int bin_encoder_64_decode_partial(const char* source, size_t src_len, unsigned char* output, size_t* result_len)
{
    int result;
    size_t decode_len;

    if (source == NULL || src_len == 0)
    {
        log_error("Invalid parameter specified");
        result = __LINE__;
    }
    else if (output == NULL)
    {
        *result_len = calculate_decoding_len(source, src_len, TYPE_BASE_64);
        result = -1;
    }
    else if ((src_len % 4) != 0)
    {
        log_error("Invalid length of base64 source");
        result = -1;
    }
    else if (*result_len < (decode_len = calculate_decoding_len(source, src_len, TYPE_BASE_64)))
    {
        *result_len = decode_len;
        log_error("Invalid length of base64 source");
        result = -1;
    }
    else
    {
        decode_base64_source_value(source, output);
        *result_len = decode_len;
        result = 0;
    }
    return result;
}
