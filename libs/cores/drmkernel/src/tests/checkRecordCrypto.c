/*
 * Copyright (C) 2014-2015 NagraVision
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

#include <unistd.h>
#include <sys/types.h>

#ifdef DRM_HOST_KERNEL_TEST
typedef __off64_t off64_t;
#endif

#include "../DrmKernel.c"

#undef ALOGV
#define ALOGV(fmt...) 
#undef ALOGE
#define ALOGE(fmt...) 


/*
 * Stubbed for avoiding too much libraries for test
 * More they can't be linked for host exe
 */
int __android_log_print(int prio, const char *tag,  const char *fmt, ...)
{
  if (getenv("DEBUG"))
    {
      va_list ap;
      
      va_start(ap, fmt);
      vfprintf(stderr, fmt, ap);
      va_end(ap);
    }

  return 0;
}

int openNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  return SQLITE_OK;
}

int insertSecureRecord(sqlite3* pxDatabase, SecureRecord xRecord)
{
  return SQLITE_OK;
}

int closeNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  return SQLITE_OK;
}

int selectSecureRecord(sqlite3* pxDatabase, const char* xKey, SecureRecord* pxRecord)
{
  return SQLITE_OK;
}


/*
 * These are test vectors
 * tag1 (resp. 2) is the aes mac for record1 (resp. 2) 
 */
const char *record1 = "abcdefghijklmnopqrstuvwxyz012345";
const uint8_t tag1[SHA256_DIGEST_LENGTH] = {
  /*
    0000000 e6 1f c4 a8 4e 02 59 71 47 20 d9 98 b7 a0 40 da
    0000020 4a 42 f5 7a 8a 06 f2 d1 5b d1 6d 9d 4d 96 ef 5a
  */
  0xe6, 0x1f, 0xc4, 0xa8, 0x4e, 0x02, 0x59, 0x71, 0x47, 0x20, 0xd9, 0x98, 0xb7, 0xa0, 0x40, 0xda, 
  0x4a, 0x42, 0xf5, 0x7a, 0x8a, 0x06, 0xf2, 0xd1, 0x5b, 0xd1, 0x6d, 0x9d, 0x4d, 0x96, 0xef, 0x5a
};

const char *record2 = "6abcdefghijklmnopqrstuvwxyz01234";
const uint8_t tag2[SHA256_DIGEST_LENGTH] = {
  /*
    0000000 10 f6 c9 4c f0 cc 47 79 30 fa 83 a8 13 aa 6b 7e
    0000020 96 9c d6 15 8f b6 4d 43 29 6e e7 ce 5e fa 48 d2
  */
  0x10, 0xf6, 0xc9, 0x4c, 0xf0, 0xcc, 0x47, 0x79, 0x30, 0xfa, 0x83, 0xa8, 0x13, 0xaa, 0x6b, 0x7e,
  0x96, 0x9c, 0xd6, 0x15, 0x8f, 0xb6, 0x4d, 0x43, 0x29, 0x6e, 0xe7, 0xce, 0x5e, 0xfa, 0x48, 0xd2
};

int main()
{
  SecureRecord mRecord1;
  SecureRecord mRecord2;
  SecureRecord mRecord3;
  SecureRecord mRecord4;
  int ret = 0;

  mRecord1._key = "record1";
  mRecord1._data = (unsigned char *)record1;
  mRecord1._dataSize = strlen(record1);
  mRecord1._tag = (unsigned char *)tag1;
  mRecord1._tagSize = SHA256_DIGEST_LENGTH;

  ret = checkRecordCrypto(&mRecord1);
  printf ("===> record1 vs tag1: %s (OK expected) => %s\n", ret ? "OK":"NOK", ret ? "PASSED":"FAILED");

  mRecord2._key = "record2";
  mRecord2._data = (unsigned char *)record2;
  mRecord2._dataSize = strlen(record2);
  mRecord2._tag = (unsigned char *)tag2;
  mRecord2._tagSize = SHA256_DIGEST_LENGTH;

  ret = checkRecordCrypto(&mRecord2);
  printf ("===> record2 vs tag2: %s (OK expected) => %s\n", ret ? "OK":"NOK", ret ? "PASSED":"FAILED");

  mRecord3._key = "record1";
  mRecord3._data = (unsigned char *)record1;
  mRecord3._dataSize = strlen(record1);
  mRecord3._tag = (unsigned char *)tag2;
  mRecord3._tagSize = SHA256_DIGEST_LENGTH;

  ret = checkRecordCrypto(&mRecord3);
  printf ("===> record1 vs tag2: %s (NOK expected) => %s\n", ret ? "OK":"NOK", ret ? "FAILED":"PASSED");

  mRecord4._key = "record2";
  mRecord4._data = (unsigned char *)record2;
  mRecord4._dataSize = strlen(record2);
  mRecord4._tag = (unsigned char *)tag1;
  mRecord4._tagSize = SHA256_DIGEST_LENGTH;

  ret = checkRecordCrypto(&mRecord4);
  printf ("===> record2 vs tag1: %s (NOK expected) => %s\n", ret ? "OK":"NOK", ret ? "FAILED":"PASSED");

  return(0);
}
