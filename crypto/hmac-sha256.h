/*
 * hmac-sha256.h
 * Copyright (C) 2017 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef HMAC_SHA256_H
#define HMAC_SHA256_H

 /*************************** HEADER FILES ***************************/
#include <stddef.h>

 /****************************** MACROS ******************************/
#define HMAC_SHA256_BLOCK_SIZE 32            // HMAC SHA256 outputs a 32 byte digest

 /**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

void
hmac_sha256 (BYTE out[HMAC_SHA256_BLOCK_SIZE],
             const BYTE *data, size_t data_len,
             const BYTE *key, size_t key_len);

#endif /* !HMAC_SHA256_H */
