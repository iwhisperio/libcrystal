/*
 * Copyright (c) 2017-2018 iwhisper.io
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __CRYSTAL_CRYPTO_H__
#define __CRYSTAL_CRYPTO_H__

#include <stdint.h>

#include <crystal/crystal_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_BYTES            32U
#define NONCE_BYTES             24U
#define PUBLIC_KEY_BYTES        32U
#define SECRET_KEY_BYTES        32U
#define SYMMETRIC_KEY_BYTES     32U
#define MAC_BYTES               16U
#define ZERO_BYTES              32U

/**
 * SHA256 hash digest.
 *
 * The result should compatible with Linux command "shasum -a 256".
 *
 * @param
 *      data        [in] The data buffer to be digest.
 * @param
 *      len         [in] The data length in the buffer.
 * @param
 *      digest      [out] The digest result buffer, caller provided.
 * @param
 *      digestlen   [in] The digest buffer size.
 *
 * @return
 *      The digest string, or NULL if the digest buffer too small.
 */
CRYSTAL_API
char *sha256a(const void *data, size_t len, char *digest, size_t digestlen);

/**
 * SHA256 hash digest.
 *
 * The result should compatible with Linux command "shasum -a 256".
 *
 * @param
 *      data        [in] The data buffer to be digest.
 * @param
 *      len         [in] The data length in the buffer.
 * @param
 *      digest      [out] The digest result buffer, caller provided.
 * @param
 *      digestlen   [in] The digest buffer size.
 *
 * @return
 *      The length of result digest bytes, or -1 if digest buffer too small.
 */
CRYSTAL_API
ssize_t sha256(const void *data, size_t len, unsigned char *digest, size_t digestlen);

/**
 * Hash-based message authentication code with SHA256.
 *
 * The result should compatible with Linux command "openssl dgst -sha256 -hmac".
 *
 * @param
 *      key         [in] The secret key buffer.
 * @param
 *      keylen      [in] The secret key length.
 * @param
 *      msg         [in] The message to be authenticate.
 * @param
 *      msglen      [in] The message length.
 * @param
 *      digest      [out] The digest result buffer, caller provided.
 * @param
 *      digestlen   [in] The digest buffer size.
 *
 * @return
 *      The digest string, or NULL if the digest buffer too small.
 */
CRYSTAL_API
char *hmac_sha256a(const void *key, size_t keylen,
                   const void *msg, size_t msglen,
                   char *digest, size_t digestlen);

/**
 * Hash-based message authentication code with SHA256.
 *
 * The result should compatible with Linux command "openssl dgst -sha1 -hmac".
 *
 * @param
 *      key         [in] The secret key buffer.
 * @param
 *      keylen      [in] The secret key length.
 * @param
 *      msg         [in] The message to be authenticate.
 * @param
 *      msglen      [in] The message length.
 * @param
 *      digest      [out] The digest result buffer, caller provided.
 * @param
 *      digestlen   [in] The digest buffer size.
 *
 * @return
 *      The length of result digest bytes, or -1 if digest buffer too small.
 */
CRYSTAL_API
ssize_t hmac_sha256(const void *key, size_t keylen,
                    const void *msg, size_t msglen,
                    unsigned char *digest, size_t digestlen);

CRYSTAL_API
void crypto_random_nonce(uint8_t *nonce);

CRYSTAL_API
char *crypto_nonce_to_str(const uint8_t *nonce, char *buf, size_t len);

CRYSTAL_API
uint8_t *crypto_nonce_from_str(uint8_t *nonce, const char *buf, size_t len);

CRYSTAL_API
void crypto_compute_symmetric_key(const uint8_t *public_key,
                                  const uint8_t *secret_key, uint8_t *key);

CRYSTAL_API
ssize_t crypto_encrypt(const uint8_t *key, const uint8_t *nonce,
                       const uint8_t *plain, size_t length, uint8_t *encrypted);

CRYSTAL_API
ssize_t crypto_encrypt2(const uint8_t *key, const uint8_t *nonce,
                        uint8_t *plain, size_t length, uint8_t *encrypted);

CRYSTAL_API
ssize_t crypto_decrypt(const uint8_t *key, const uint8_t *nonce,
                       const uint8_t *encrypted, size_t length, uint8_t *plain);

CRYSTAL_API
ssize_t crypto_decrypt2(const uint8_t *key, const uint8_t *nonce,
                        uint8_t *encrypted, size_t length, uint8_t *plain);

CRYSTAL_API
int crypto_create_keypair(uint8_t *public_key, uint8_t *secret_key);

#ifdef __cplusplus
}
#endif

#endif /* __CRYSTAL_CRYPTO_H__ */
