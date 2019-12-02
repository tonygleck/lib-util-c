// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef BINARY_ENCODER_H
#define BINARY_ENCODER_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

#include "umock_c/umock_c_prod.h"

/**
* @brief    Encodes the unsigned char to a base 32 char string
*
* @param    source   An unsigned char* to be encoded
* @param    size     The lenght in bytes of the source variable
* @param    result   The resulting encoding of the binary data
* @param    size     The lenght of bytes of the result string
*
* @return   An integer indicating if the encoding was successful.  Zero indicates success, any other value is failure and
*           the amount of memory needed
*/
MOCKABLE_FUNCTION(, int, bin_encoder_32_encode, const unsigned char*, source, size_t, size, char*, result, size_t*, result_len);

/**
* @brief    Decodes a base 32 encoded char* to an unsigned char
*
* @param    source   char* of a base 32 encode string
* @param    result   The resulting decoding of the string data
* @param    size     The lenght of bytes of the result unsigned char
*
* @return   An integer indicating if the encoding was successful.  Zero indicates success, any other value is failure and
*           the amount of memory needed
*/
MOCKABLE_FUNCTION(, int, bin_encoder_32_decode, const char*, source, unsigned char*, result, size_t*, result_len);

/**
* @brief    Decodes a base 32 encoded char* to an unsigned char
*
* @param    source   char* of a base 32 encode string
* @param    len      The lenght of the source to encode
* @param    result   The resulting decoding of the string data
* @param    size     The lenght of bytes of the result unsigned char
*
* @return   An integer indicating if the encoding was successful.  Zero indicates success, any other value is failure and
*           the amount of memory needed
*/
MOCKABLE_FUNCTION(, int, bin_encoder_32_decode_partial, const char*, source, size_t, len, unsigned char*, result, size_t*, result_len);

/**
* @brief    Encodes the unsigned char to a base 64 char string
*
* @param    source   An unsigned char* to be encoded
* @param    len      The lenght in bytes of the source variable
* @param    result   The resulting encoding of the binary data
* @param    size     The lenght of bytes of the result string
*
* @return   An integer indicating if the encoding was successful.  Zero indicates success, any other value is failure and
*           the amount of memory needed
*/
MOCKABLE_FUNCTION(, int, bin_encoder_64_encode, const unsigned char*, source, size_t, size, char*, result, size_t*, result_len);

/**
* @brief    Decodes a base 64 encoded char* to an unsigned char
*
* @param    source   char* of a base 64 encode string
* @param    result   The resulting decoding of the string data
* @param    size     The lenght of bytes of the result unsigned char
*
* @return   An integer indicating if the encoding was successful.  Zero indicates success, any other value is failure and
*           the amount of memory needed
*/
MOCKABLE_FUNCTION(, int, bin_encoder_64_decode, const char*, source, unsigned char*, result, size_t*, result_len);

/**
* @brief    Decodes a base 64 encoded char* to an unsigned char
*
* @param    source   char* of a base 64 encode string
* @param    len      The lenght in bytes of the source variable
* @param    result   The resulting decoding of the string data
* @param    size     The lenght of bytes of the result unsigned char
*
* @return   An integer indicating if the encoding was successful.  Zero indicates success, any other value is failure and
*           the amount of memory needed
*/
MOCKABLE_FUNCTION(, int, bin_encoder_64_decode_partial, const char*, source, size_t, len, unsigned char*, result, size_t*, result_len);

#ifdef __cplusplus
}
#endif

#endif // BINARY_ENCODER_H
