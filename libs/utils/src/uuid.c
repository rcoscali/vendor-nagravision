/*
 * Copyright (C) 2014 NagraVision
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "uuid.h"

//#define LOG_NDEBUG 0
#define LOG_TAG "nvUuidUtils"
#include <utils/Log.h>

/* ==========================================================================
 * Static definitions for UUID utils
 * ==========================================================================*/

/* status */
#define NV_KO   -1
#define NV_OK     0

/* bool */
#define NV_FALSE (1 != 1)
#define NV_TRUE  (1 == 1)

/*
 * hex2num
 *
 * Convert 1 hex digit in a byte (0 to 15)
 */
static int8_t 
hex2num(char c)
{
  if (c >= '0' && c <= '9') 
    return (int8_t)(c - '0');
  if (c >= 'a' && c <= 'f') 
    return (int8_t)(c - 'a' + 10);
  if (c >= 'A' && c <= 'F') 
    return (int8_t)(c - 'A' + 10);

  return (int8_t)NV_KO;
}

/*
 * hex2num
 *
 * Convert hex encoded byte to a byte
 */
static int16_t
hex2byte(const char *hex)
{
  int8_t high = hex2num(*hex++);
  if (high < 0) return NV_KO;

  int8_t low = hex2num(*hex);
  if (low < 0) return NV_KO;

  return (high << 4) | low;
}

static int8_t
hexstr2bin(const char    *hex, 
	         uint8_t *buf, 
	         size_t   len)
{
  int i = 0;
  int16_t a = 0;
  const char *inptr = hex;
  uint8_t *outptr = buf;

  for (i = 0; i < (int)len; i++) 
    {
      a = hex2byte(inptr++);
      if (a < 0) return NV_KO;
      *outptr++ = a;
    }

  return NV_OK;
}

/* ==========================================================================
 * Implementation of UUID utils
 * ==========================================================================*/

/*
 * uuid_str2bin
 *
 * Convert an UUID in hex string format to a 16 bytes array
 *
 * from: 00000000-0000-0000-0000-000000000000
 *   to: { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
 */
int8_t 
uuid_str2bin(const char    *str, 
	           uint8_t *bin)
{
  const char *inptr = str;
  uint8_t *outptr = bin;
  int sz;

#define ADVANCE_IN       inptr++
#define ADVANCE_BOTH     inptr += sz*2; outptr += sz

  /*  Parse uuid string: first four bytes */
  sz = 4;
  if (hexstr2bin(inptr, outptr, sz)) return NV_KO;
  ADVANCE_BOTH;

  /* skip dash then next 2 bytes */
  if (*inptr != '-') return NV_KO;
  ADVANCE_IN;
  sz = 2;
  if (hexstr2bin(inptr, outptr, sz)) return NV_KO;
  ADVANCE_BOTH;

  /* skip dash then next 2 bytes */
  if (*inptr != '-') return NV_KO;
  ADVANCE_IN;
  sz = 2;
  if (hexstr2bin(inptr, outptr, sz)) return NV_KO;
  ADVANCE_BOTH;

  /* skip dash then next 2 bytes */
  if (*inptr != '-') return NV_KO;
  ADVANCE_IN;
  sz = 2;
  if (hexstr2bin(inptr, outptr, sz)) return NV_KO;
  ADVANCE_BOTH;

  /* skip dash then last 6 bytes */
  if (*inptr != '-') return NV_KO;
  ADVANCE_IN;
  sz = 6;
  if (hexstr2bin(inptr, outptr, sz)) return NV_KO;
  ADVANCE_BOTH;

#undef ADVANCE_IN
#undef ADVANCE_BOTH

  return NV_OK;
}

/*
 * uuid_bin2str
 *
 * Convert an UUID in 16 bytes array format to an hex string
 *
 * from: { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
 *   to: 00000000-0000-0000-0000-000000000000
 */
int8_t
uuid_bin2str(const uint8_t *bin, 
	           char    *str, 
	           size_t   max_len)
{
  int len = -1;
  len = snprintf(str, 
		 max_len, 
		 "%02x%02x%02x%02x-"                  /*  4 digits  9 chars */
		 "%02x%02x-"                          /*  2 digits  5 chars */
		 "%02x%02x-"                          /*  2 digits  5 chars */
		 "%02x%02x-"                          /*  2 digits  5 chars */
		 "%02x%02x%02x%02x%02x%02x",          /*  6 digits 12 chars */
		 /* 16 digits total */
		 bin[0], bin[1], bin[2], bin[3],
		 bin[4], bin[5], 
		 bin[6], bin[7],
		 bin[8], bin[9], 
		 bin[10], bin[11], bin[12], bin[13], bin[14], bin[15]);
  if (len < 0 || (size_t)len >= max_len)
    return NV_KO;

  return NV_OK;
}

/*
 * uuid_is_nil
 *
 * Return true if UUID is nil (00000000-0000-0000-0000-000000000000)
 */
int8_t
uuid_is_nil(const uint8_t *uuid)
{
  int i;
  for (i = 0; i < UUID_LEN; i++)
    if (uuid[i])
      return NV_FALSE;

  return NV_TRUE;
}

/*
 * uuid_cmp
 *
 * Compare two UUIDs in 16 bytes array format
 */
int8_t
uuid_cmp(const uint8_t *uuid1, const uint8_t *uuid2)
{
  return memcmp((const void *)uuid1, (const void *)uuid2, UUID_LEN);
}
