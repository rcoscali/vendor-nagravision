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

/* for generating a content right:
   # echo -n "<content id>" | openssl sha256 -binary | openssl enc -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | od -A n -w32 -t x1
   this command dump the content right as a hex byte string (without 0x).
   ex.:

       cohen@PC12316-LX:~$ echo -n "6abcdefghijklmnopqrstuvwxyz01234" | openssl sha256 -binary | \
       > openssl enc -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | od -A n -w32 -t x1
        10 f6 c9 4c f0 cc 47 79 30 fa 83 a8 13 aa 6b 7e 96 9c d6 15 8f b6 4d 43 29 6e e7 ce 5e fa 48 d2
       cohen@PC12316-LX:~$ 
   
   Key and IV are the one defined in DrmKernel.c provided to openssl as hex bytes string

   Test vectors are:

       cohen@PC12316-LX:~$ echo -n "A Title for a content 1" | openssl sha256 -binary | \
       > openssl enc -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | od -A n -w32 -t x1
        33 a2 9b a2 8c b4 0a 74 64 d2 4d 14 92 bd d2 44 de a2 13 8e 41 dd 28 9f f7 31 fe d9 9f 66 9e 4d
       cohen@PC12316-LX:~$ 

       cohen@PC12316-LX:~$ echo -n "A Title for a content 2" | openssl sha256 -binary | \
       > openssl enc -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | od -A n -w32 -t x1
        b2 f8 c1 39 a4 a9 57 ca cf a2 e3 c4 fb 76 0f ef a1 c0 a1 0c 4c 2a ce 31 df 00 24 0c 60 57 a1 53
       cohen@PC12316-LX:~$ 

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

const uint8_t right1_right_data[SHA256_DIGEST_LENGTH] = {
  /* 0x00 */  0x33, 0xa2, 0x9b, 0xa2, 0x8c, 0xb4, 0x0a, 0x74, 
  /* 0x08 */  0x64, 0xd2, 0x4d, 0x14, 0x92, 0xbd, 0xd2, 0x44, 
  /* 0x10 */  0xde, 0xa2, 0x13, 0x8e, 0x41, 0xdd, 0x28, 0x9f, 
  /* 0x18 */  0xf7, 0x31, 0xfe, 0xd9, 0x9f, 0x66, 0x9e, 0x4d
};
const size_t right1_right_datalen = SHA256_DIGEST_LENGTH;
const uint8_t right2_right_data[SHA256_DIGEST_LENGTH] = {
  /* 0x00 */  0xb2, 0xf8, 0xc1, 0x39, 0xa4, 0xa9, 0x57, 0xca, 
  /* 0x08 */  0xcf, 0xa2, 0xe3, 0xc4, 0xfb, 0x76, 0x0f, 0xef, 
  /* 0x10 */  0xa1, 0xc0, 0xa1, 0x0c, 0x4c, 0x2a, 0xce, 0x31, 
  /* 0x18 */  0xdf, 0x00, 0x24, 0x0c, 0x60, 0x57, 0xa1, 0x53
};
const size_t right2_right_datalen = SHA256_DIGEST_LENGTH;

const char *contentId1 = "A Title for a content 1";
const char *contentId2 = "A Title for a content 2";

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
  if (strcmp(xKey, contentId1) == 0)
    {
      pxRecord->_data = strdup(contentId1);
      pxRecord->_dataSize = strlen(contentId1);
      pxRecord->_tag = right1_right_data;
      pxRecord->_tagSize = right1_right_datalen;
    }
  else if (strcmp(xKey, contentId2) == 0)
    {
      pxRecord->_data = strdup(contentId2);
      pxRecord->_dataSize = strlen(contentId2);
      pxRecord->_tag = right2_right_data;
      pxRecord->_tagSize = right2_right_datalen;
    }
  return SQLITE_OK;
}

int main()
{
  status_t ret;

  ret = DrmKernel_NvDrmPlugin_onCheckRightsStatus(1, contentId1, 0);
  printf("===> Check right1 for contentId1: %s (OK expected) => %s\n", ret == NV_RightsStatus_RIGHTS_VALID ? "OK" : "NOK", ret == NV_RightsStatus_RIGHTS_VALID ? "PASSED" : "FAILED");
  ret = DrmKernel_NvDrmPlugin_onCheckRightsStatus(1, contentId1, 0);
  printf("===> Check right2 for contentId2: %s (OK expected) => %s\n", ret == NV_RightsStatus_RIGHTS_VALID ? "OK" : "NOK", ret == NV_RightsStatus_RIGHTS_VALID ? "PASSED" : "FAILED");

  return(0);
}
