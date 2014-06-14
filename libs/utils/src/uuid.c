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

static int 
hex2num(char c)
{
  if (c >= '0' && c <= '9') return (c - '0');
  if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
  if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
  return NV_KO;
}

static int 
hex2byte(const char *hex)
{
  int a = hex2num(*hex++);
  if (a < 0) return NV_KO;
  int b = hex2num(*hex++);
  if (b < 0) return NV_KO;
  return (a << 4) | b;
}

static int 
hexstr2bin(const char *hex, 
	     uint8_t *buf, 
	     size_t len)
{
  size_t i = 0;
  int a = 0;
  const char *ipos = hex;
  uint8_t *opos = buf;

  for (i = 0; i < len; i++) {
    a = hex2byte(ipos);
    if (a < 0)
      return NV_KO;
    *opos++ = a;
    ipos += 2;
  }
  return NV_OK;
}

/* ==========================================================================
 * Implementation of UUID utils
 * ==========================================================================*/

int 
uuid_str2bin(const char *str, 
	     uint8_t *bin)
{
  const char *pos = (const char *)NULL;
  uint8_t *opos = (uint8_t *)NULL;

  pos = str; opos = bin;
  if (hexstr2bin(pos, opos, 4)) return NV_KO;
  pos += 8; opos += 4;
  if (*pos++ != '-' || hexstr2bin(pos, opos, 2)) return NV_KO;
  pos += 4; opos += 2;
  if (*pos++ != '-' || hexstr2bin(pos, opos, 2)) return NV_KO;
  pos += 4; opos += 2;
  if (*pos++ != '-' || hexstr2bin(pos, opos, 2)) return NV_KO;
  pos += 4; opos += 2;
  if (*pos++ != '-' || hexstr2bin(pos, opos, 6)) return NV_KO;
  return NV_OK;
}


int 
uuid_bin2str(const uint8_t *bin, 
	     char *str, 
	     size_t max_len)
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
  if (len < 0 || (size_t) len >= max_len)
    return NV_KO;
  return NV_OK;
}

int
uuid_cmp(const uint8_t *uuid1, const uint8_t *uuid2)
{
  int uuid1_is_nil = uuid_is_nil(uuid1);
  int uuid2_is_nil = uuid_is_nil(uuid2);

  if (uuid1_is_nil && uuid2_is_nil)
    return NV_TRUE;
  if((uuid1_is_nil && !uuid2_is_nil) ||
     (!uuid1_is_nil && uuid2_is_nil))
    return NV_FALSE;
  char uuid1_str[UUID_STR_LEN];
  char uuid2_str[UUID_STR_LEN];
  if (uuid_bin2str(uuid1, uuid1_str, UUID_STR_LEN) != NV_OK) return NV_KO;
  if (uuid_bin2str(uuid2, uuid2_str, UUID_STR_LEN) != NV_OK) return NV_KO;
  return (strncmp(uuid1_str, uuid2_str, UUID_STR_LEN));
}

int 
uuid_is_nil(const uint8_t *uuid)
{
  int i = 0;
  for (i = 0; i < UUID_LEN; i++)
    if (uuid[i])
      return NV_FALSE;
  return NV_TRUE;
}
