// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <stdlib.h>
#include <stdio.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/app_logging.h"
#include "lib-util-c/sha_algorithms.h"
#include "lib-util-c/sha256_impl.h"

#define SHA_256_MSG_BLOCK_SIZE      64

typedef struct SHA_CTX_256_TAG
{
    uint32_t intermediate_hash[SHA256_HASH_SIZE/4]; // Message Digest
    uint32_t len_low;                // Message length in bits
    uint32_t len_high;               // Message length in bits

    int_least16_t msg_block_index;  // Message_Block array index 512-bit message blocks */
    uint8_t msg_block[SHA_256_MSG_BLOCK_SIZE];
    int is_computed;    // Is the digest computed?
    int is_corrupted;   // Is the digest corrupted?
} SHA_CTX_256;

// Initial Hash Values: FIPS-180-2 section 5.3.2
static uint32_t SHA256_H0[SHA256_HASH_SIZE/4] = {
    0x6A09E667, 0xBB67AE85,
    0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C,
    0x1F83D9AB, 0x5BE0CD19
};


// These definitions are defined in FIPS-180-2, section 4.1.
// Ch() and Maj() are defined identically in sections 4.1.1,
// 4.1.2 and 4.1.3.
// The definitions used in FIPS-180-2 are as follows:
#ifndef USE_MODIFIED_MACROS
#define SHA_Ch(x,y,z)        (((x) & (y)) ^ ((~(x)) & (z)))
#define SHA_Maj(x,y,z)       (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#else // USE_MODIFIED_MACROS
// The following definitions are equivalent and potentially faster.
#define SHA_Ch(x, y, z)      (((x) & ((y) ^ (z))) ^ (z))
#define SHA_Maj(x, y, z)     (((x) & ((y) | (z))) | ((y) & (z)))
#endif // USE_MODIFIED_MACROS

#define SHA_Parity(x, y, z)  ((x) ^ (y) ^ (z))

// add "length" to the length
#define SHA224_256AddLength(sha_ctx, length)                \
  (add_temp = (sha_ctx)->len_low, (sha_ctx)->is_corrupted = \
    (((sha_ctx)->len_low += (length)) < add_temp) &&        \
    (++(sha_ctx)->len_high == 0) ? 1 : 0)

// Define the SHA shift, rotate left and rotate right macro
#define SHA256_SHR(bits,word)      ((word) >> (bits))
#define SHA256_ROTL(bits,word)                         \
  (((word) << (bits)) | ((word) >> (32-(bits))))
#define SHA256_ROTR(bits,word)                         \
  (((word) >> (bits)) | ((word) << (32-(bits))))

// Define the SHA SIGMA and sigma macros
#define SHA256_SIGMA0(word)   \
  (SHA256_ROTR( 2,word) ^ SHA256_ROTR(13,word) ^ SHA256_ROTR(22,word))
#define SHA256_SIGMA1(word)   \
  (SHA256_ROTR( 6,word) ^ SHA256_ROTR(11,word) ^ SHA256_ROTR(25,word))
#define SHA256_sigma0(word)   \
  (SHA256_ROTR( 7,word) ^ SHA256_ROTR(18,word) ^ SHA256_SHR( 3,word))
#define SHA256_sigma1(word)   \
  (SHA256_ROTR(17,word) ^ SHA256_ROTR(19,word) ^ SHA256_SHR(10,word))

// This function will process the next 512 bits of the message stored in the Message_Block array.
static void sha256_process_msg_block(SHA_CTX_256* sha_ctx)
{
    // Constants defined in FIPS-180-2, section 4.2.2
    static const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
        0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
        0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
        0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
        0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
        0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
        0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
        0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
        0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
        0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    uint32_t temp1, temp2;            // Temporary word value
    uint32_t W[64];                   // Word sequence
    uint32_t A, B, C, D, E, F, G, H;  // Word buffers

    // Initialize the first 16 words in the array W
    size_t inner;
    for (size_t index = inner = 0; index < 16; index++, inner += 4)
    {
        W[index] = (((uint32_t)sha_ctx->msg_block[inner]) << 24) |
            (((uint32_t)sha_ctx->msg_block[inner + 1]) << 16) |
            (((uint32_t)sha_ctx->msg_block[inner + 2]) << 8) |
            (((uint32_t)sha_ctx->msg_block[inner + 3]));
    }

    for (size_t index = 16; index < 64; index++)
    {
        W[index] = SHA256_sigma1(W[index - 2]) + W[index - 7] +
            SHA256_sigma0(W[index - 15]) + W[index - 16];
    }

    A = sha_ctx->intermediate_hash[0];
    B = sha_ctx->intermediate_hash[1];
    C = sha_ctx->intermediate_hash[2];
    D = sha_ctx->intermediate_hash[3];
    E = sha_ctx->intermediate_hash[4];
    F = sha_ctx->intermediate_hash[5];
    G = sha_ctx->intermediate_hash[6];
    H = sha_ctx->intermediate_hash[7];

    for (size_t index = 0; index < 64; index++)
    {
        temp1 = H + SHA256_SIGMA1(E) + SHA_Ch(E, F, G) + K[index] + W[index];
        temp2 = SHA256_SIGMA0(A) + SHA_Maj(A, B, C);
        H = G;
        G = F;
        F = E;
        E = D + temp1;
        D = C;
        C = B;
        B = A;
        A = temp1 + temp2;
    }

    sha_ctx->intermediate_hash[0] += A;
    sha_ctx->intermediate_hash[1] += B;
    sha_ctx->intermediate_hash[2] += C;
    sha_ctx->intermediate_hash[3] += D;
    sha_ctx->intermediate_hash[4] += E;
    sha_ctx->intermediate_hash[5] += F;
    sha_ctx->intermediate_hash[6] += G;
    sha_ctx->intermediate_hash[7] += H;
    sha_ctx->msg_block_index = 0;
}

static void pad_256_msg(SHA_CTX_256* sha_ctx, unsigned char pad_byte)
{

    // Check to see if the current message block is too small to hold
    // the initial padding bits and length. If so, we will pad the
    // block, process it, and then continue padding into a second
    // block.
    if (sha_ctx->msg_block_index >= (SHA_256_MSG_BLOCK_SIZE - 8))
    {
        sha_ctx->msg_block[sha_ctx->msg_block_index++] = pad_byte;
        while (sha_ctx->msg_block_index < SHA_256_MSG_BLOCK_SIZE)
        {
            sha_ctx->msg_block[sha_ctx->msg_block_index++] = 0;
        }
        sha256_process_msg_block(sha_ctx);
    }
    else
    {
        sha_ctx->msg_block[sha_ctx->msg_block_index++] = pad_byte;
    }

    while (sha_ctx->msg_block_index < (SHA_256_MSG_BLOCK_SIZE - 8))
    {
        sha_ctx->msg_block[sha_ctx->msg_block_index++] = 0;
    }

    // Store the message length as the last 8 octets
    sha_ctx->msg_block[56] = (uint8_t)(sha_ctx->len_high >> 24);
    sha_ctx->msg_block[57] = (uint8_t)(sha_ctx->len_high >> 16);
    sha_ctx->msg_block[58] = (uint8_t)(sha_ctx->len_high >> 8);
    sha_ctx->msg_block[59] = (uint8_t)(sha_ctx->len_high);
    sha_ctx->msg_block[60] = (uint8_t)(sha_ctx->len_low >> 24);
    sha_ctx->msg_block[61] = (uint8_t)(sha_ctx->len_low >> 16);
    sha_ctx->msg_block[62] = (uint8_t)(sha_ctx->len_low >> 8);
    sha_ctx->msg_block[63] = (uint8_t)(sha_ctx->len_low);

    sha256_process_msg_block(sha_ctx);
}

static int sha256_retrieve_result(SHA_IMPL_HANDLE sha_handle, uint8_t msg_digest[], size_t digest_len)
{
    int result;
    if (sha_handle == NULL || msg_digest == NULL || digest_len == 0)
    {
        log_error("Invalid parameter specified sha_handle: %p, msg_digest: %p, digest_len: %zu", sha_handle, msg_digest, digest_len);
        result = __LINE__;
    }
    else if (digest_len < SHA256_HASH_SIZE)
    {
        log_error("Insufficient msg_digest size.  Expected %d", SHA256_HASH_SIZE);
        result = __LINE__;
    }
    else
    {
        SHA_CTX_256* sha_ctx = (SHA_CTX_256*)sha_handle;
        if (sha_ctx->is_corrupted)
        {
            log_error("sha value is corrupted");
            result = __LINE__;
        }
        else
        {
            result = 0;
            if (!sha_ctx->is_computed)
            {
                //finalize_256_result(sha_ctx, 0x80);
                pad_256_msg(sha_ctx, 0x80);
                // message may be sensitive, so clear it out
                for (size_t index = 0; index < SHA_256_MSG_BLOCK_SIZE; ++index)
                {
                    sha_ctx->msg_block[index] = 0;
                }
                sha_ctx->len_low = 0;  // and clear length
                sha_ctx->len_high = 0;
                sha_ctx->is_computed = 1;
            }
            for (size_t index = 0; index < digest_len; index++)
            {
                msg_digest[index] = (uint8_t)(sha_ctx->intermediate_hash[index >> 2] >> 8 * (3 - (index & 0x03)));
            }
        }
    }
    return result;
}

static int sha256_process_hash(SHA_IMPL_HANDLE sha_handle, const uint8_t* msg_array, size_t array_len)
{
    int result;
    if (sha_handle == NULL || msg_array == NULL || array_len == 0)
    {
        log_error("Invalid parameter specified sha_handle: %p, msg_array: %p, array_len: %zu", sha_handle, msg_array, array_len);
        result = __LINE__;
    }
    else
    {
        SHA_CTX_256* sha_ctx = (SHA_CTX_256*)sha_handle;
        // Only compute the hash once and if we're errored then fail
        if (sha_ctx->is_computed || sha_ctx->is_corrupted)
        {
            log_error("sha value is corrupted");
            result = __LINE__;
        }
        else
        {
            uint32_t add_temp;

            result = 0;
            while (array_len-- && !sha_ctx->is_corrupted)
            {
                sha_ctx->msg_block[sha_ctx->msg_block_index++] = (*msg_array & 0xFF);
                if (!SHA224_256AddLength(sha_ctx, 8) && (sha_ctx->msg_block_index == SHA_256_MSG_BLOCK_SIZE))
                {
                    sha256_process_msg_block(sha_ctx);
                }
                msg_array++;
            }
        }
    }
    return result;
}

static SHA_IMPL_HANDLE sha256_initialize(void)
{
    SHA_CTX_256* result;
    if ((result = malloc(sizeof(SHA_CTX_256))) == NULL)
    {
        log_error("Failure allocating Sha256 structure");
    }
    else
    {
        memset(result, 0, sizeof(SHA_CTX_256));
        for (size_t index = 0; index < 8; index++)
        {
            result->intermediate_hash[index] = SHA256_H0[index];
        }
    }
    return result;
}

static void sha256_deinit(SHA_IMPL_HANDLE handle)
{
    if (handle != NULL)
    {
        free(handle);
    }
}

static SHA_HASH_INTERFACE sha_interface =
{
    sha256_initialize,
    sha256_deinit,
    sha256_process_hash,
    sha256_retrieve_result
};

const SHA_HASH_INTERFACE* sha256_get_interface(void)
{
    return &sha_interface;
}
