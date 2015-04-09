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

/* 
   For generating a protected content right key, 
   key being: [0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf]

       cohen@PC12316-LX:~$ echo -ne "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf" | \
       > openssl enc -e -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | \
       > od -A n -w32 -t x1
        91 a9 ba a6 ba 28 e5 15 7f 60 23 c7 79 53 21 16 c2 97 36 e0 6f 3b 73 65 d0 81 2a 86 0e 4b f7 10
       cohen@PC12316-LX:~$

   key being: [0x7f, 0x99, 0x06, 0x95, 0x78, 0xab, 0x4d, 0xae, 0x8d, 0x6c, 0xe2, 0x4c, 0xc3, 0x21, 0x02, 0x32]

       cohen@PC12316-LX:~$ echo -ne "\x7f\x99\x06\x95\x78\xab\x4d\xae\x8d\x6c\xe2\x4c\xc3\x21\x02\x32" | \
       > openssl enc -e -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | \
       > od -A n -w32 -t x1
        62 20 97 23 6d 4b ad e2 12 38 a7 70 d2 17 e3 c5 7c cf 27 cc 41 36 90 06 f0 12 8c dd 29 96 e3 96
       cohen@PC12316-LX:~$

for generating a content right tag:
   # echo -ne "contentId + <length> + <protectedKey>" | openssl sha256 -binary | openssl enc -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | od -A n -w32 -t x1
   ex.:

   Key and IV are the one defined in DrmKernel.c provided to openssl as hex bytes string

   For the right 1 test vector
     "A Title for a content 1" : \x41\x20\x54\x69\x74\x6c\x65\x20\x66\x6f\x72\x20\x61\x20\x63\x6f\x6e\x74\x65\x6e\x74\x20\x31
     Length                      \x00\x20
     Protected Key #1            \x91\xa9\xba\xa6\xba\x28\xe5\x15\x7f\x60\x23\xc7\x79\x53\x21\x16\xc2\x97\x36\xe0\x6f\x3b\x73\x65\xd0\x81\x2a\x86\x0e\x4b\xf7\x10

       cohen@PC12316-LX:~$ echo -ne "\x41\x20\x54\x69\x74\x6c\x65\x20\x66\x6f\x72\x20\x61\x20\x63\x6f\x6e\x74\x65\x6e\x74\x20\x31\x00\x20\x91
       > \xa9\xba\xa6\xba\x28\xe5\x15\x7f\x60\x23\xc7\x79\x53\x21\x16\xc2\x97\x36\xe0\x6f\x3b\x73\x65\xd0\x81\x2a\x86\x0e\x4b\xf7\x10" | \
       > openssl sha256 -binary | openssl enc -e -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | \
       > od -A n -w32 -t x1
        76 6d 75 87 3c 25 14 28 68 42 b3 3e fd aa 57 dc 9c f9 fe dd 9d 65 db e7 5d 5a fc 10 fd 74 41 06
       cohen@PC12316-LX:Nitrogen6X-imx_jb4.3_1.3_NV-ga$ 

   For the right 2 test vector
     "A Title for a content 2" : \x41\x20\x54\x69\x74\x6c\x65\x20\x66\x6f\x72\x20\x61\x20\x63\x6f\x6e\x74\x65\x6e\x74\x20\x32
     Length                      \x00\x20
     Protected Key #2            \x62\x20\x97\x23\x6d\x4b\xad\xe2\x12\x38\xa7\x70\xd2\x17\xe3\xc5\x7c\xcf\x27\xcc\x41\x36\x90\x06\xf0\x12\x8c\xdd\x29\x96\xe3\x96

       cohen@PC12316-LX:~$ echo -ne "\x41\x20\x54\x69\x74\x6c\x65\x20\x66\x6f\x72\x20\x61\x20\x63\x6f\x6e\x74\x65\x6e\x74\x20\x32\x00\x20\
       > \x62\x20\x97\x23\x6d\x4b\xad\xe2\x12\x38\xa7\x70\xd2\x17\xe3\xc5\x7c\xcf\x27\xcc\x41\x36\x90\x06\xf0\x12\x8c\xdd\x29\x96\xe3\x96" | \
       > openssl sha256 -binary | openssl enc -e -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | \
       > od -A n -w32 -t x1
        cd 66 f7 b4 94 89 da 02 74 7b 3c ca d8 ec 4a 15 55 c2 ed 4d d7 9a 78 d3 49 98 7f 5c 47 11 56 e3
       cohen@PC12316-LX:Nitrogen6X-imx_jb4.3_1.3_NV-ga$ 

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

const uint8_t right1_right_data[26 + AES_BLOCK_SIZE*3 + SHA256_DIGEST_LENGTH] = {
  /* ContentId 0x00 */  0x41, 0x20, 0x54, 0x69, 0x74, 0x6c, 0x65, 0x20, 
  /*           0x08 */  0x66, 0x6f, 0x72, 0x20, 0x61, 0x20, 0x63, 0x6f, 
  /*           0x10 */  0x6e, 0x74, 0x65, 0x6e, 0x74, 0x20, 0x31, 0x00,
  /* KeyId     0x00 */  0x12, 0x1a, 0x0f, 0xca, 0x0f, 0x1b, 0x47, 0x5b, 
  /*           0x08 */  0x89, 0x10, 0x29, 0x7f, 0xa8, 0xe0, 0xa0, 0x7e,
  /* KeyLength 0x00 */  0x00, 0x20, 
  /* Key       0x00 */  0x91, 0xa9, 0xba, 0xa6, 0xba, 0x28, 0xe5, 0x15, 
  /*           0x08 */  0x7f, 0x60, 0x23, 0xc7, 0x79, 0x53, 0x21, 0x16, 
  /*           0x10 */  0xc2, 0x97, 0x36, 0xe0, 0x6f, 0x3b, 0x73, 0x65, 
  /*           0x18 */  0xd0, 0x81, 0x2a, 0x86, 0x0e, 0x4b, 0xf7, 0x10,
  /* SHA256    0x00 */  0x74, 0x35, 0x40, 0x1a, 0x16, 0x21, 0x28, 0x38, 
  /*           0x08 */  0x2d, 0x3a, 0xba, 0x8d, 0x11, 0xb2, 0x76, 0x56, 
  /*           0x10 */  0x65, 0xe6, 0x14, 0xde, 0x79, 0xe9, 0x4e, 0x86, 
  /*           0x18 */  0xa5, 0x18, 0xd1, 0x47, 0xf3, 0x76, 0xa4, 0xb8
};
const size_t right1_right_datalen = 26 + AES_BLOCK_SIZE*3 + SHA256_DIGEST_LENGTH;
const uint8_t right2_right_data[26 + AES_BLOCK_SIZE*3 + SHA256_DIGEST_LENGTH] = {
  /* 0x00 ContentId 0x00 */  0x41, 0x20, 0x54, 0x69, 0x74, 0x6c, 0x65, 0x20, 
  /* 0x08           0x08 */  0x66, 0x6f, 0x72, 0x20, 0x61, 0x20, 0x63, 0x6f, 
  /* 0x10           0x10 */  0x6e, 0x74, 0x65, 0x6e, 0x74, 0x20, 0x31, 0x00,
  /* 0x18 KeyId     0x00 */  0x80, 0x9a, 0x82, 0xba, 0xec, 0x0e, 0x42, 0x54, 
  /* 0x20           0x08 */  0xbf, 0x43, 0x57, 0x3e, 0xed, 0x9e, 0xac, 0x02,
  /* 0x28 KeyLength 0x00 */  0x00, 0x20, 
  /* 0x2A Key       0x00 */  0x62, 0x20, 0x97, 0x23, 0x6d, 0x4b, 0xad, 0xe2, 
  /* 0x31           0x08 */  0x12, 0x38, 0xa7, 0x70, 0xd2, 0x17, 0xe3, 0xc5, 
  /* 0x39           0x10 */  0x7c, 0xcf, 0x27, 0xcc, 0x41, 0x36, 0x90, 0x06, 
  /* 0x41           0x18 */  0xf0, 0x12, 0x8c, 0xdd, 0x29, 0x96, 0xe3, 0x96,
  /* 0x49 SHA256    0x00 */  0x50, 0x42, 0xe0, 0xe5, 0xce, 0x61, 0x1e, 0xb7,
  /* 0x51           0x08 */  0xa7, 0xc6, 0x66, 0xa8, 0x94, 0xfd, 0x02, 0x85,
  /* 0x59           0x10 */  0x81, 0xf4, 0xaa, 0x6b, 0xb4, 0x2b, 0x94, 0x57,
  /* 0x61           0x18 */  0x7e, 0x37, 0x48, 0x58, 0x19, 0x9b, 0x2e, 0xb8 
  /* 0x69           0x18 */  
};
const size_t right2_right_datalen = 26 + AES_BLOCK_SIZE*3 + SHA256_DIGEST_LENGTH;

/*
 * Stubbed for avoiding too much libraries for test
 * More they can't be linked for host exe
 */
int __android_log_print(__attribute__((unused)) int prio, 
			__attribute__((unused)) const char *tag, 
			const char *fmt, ...)
{
  if (getenv("DEBUG"))
    {
      va_list ap;

      va_start(ap, fmt);
      vfprintf(stderr, fmt, ap);
      fprintf(stderr, "\n");
      va_end(ap);
    }

  return 0;
}

/* Stubs for db */
int openNvDatabase(__attribute__((unused)) NvDatabaseConnection* pxDatabaseConnection)
{
  return SQLITE_OK;
}

int insertSecureRecord(__attribute__((unused)) sqlite3* pxDatabase, 
		       __attribute__((unused)) SecureRecord *pxRecord)
{
  return SQLITE_OK;
}

int closeNvDatabase(__attribute__((unused)) NvDatabaseConnection* pxDatabaseConnection)
{
  return SQLITE_OK;
}

int selectSecureRecord(__attribute__((unused)) sqlite3* pxDatabase, 
		       __attribute__((unused)) const char* xKey, 
		       __attribute__((unused)) const uint8_t* xKeyId, 
		       __attribute__((unused)) SecureRecord* pxRecord)
{
  return SQLITE_OK;
}



int main()
{
  const char *contentId1 = "A Title for a content 1";
  const char *contentId2 = "A Title for a content 2";
  struct NV_DrmBuffer_st buf1;
  struct NV_DrmBuffer_st buf2;
  struct NV_DrmRights_st right1;
  struct NV_DrmRights_st right2;

  buf1.data = (char *)right1_right_data;
  buf1.length = right1_right_datalen;

  right1.data = &buf1;
  right1.mimeType = "video/mp4";
  right1.accountId = 0;
  right1.subscriptionId = 0;

  buf2.data = (char *)right2_right_data;
  buf2.length = right2_right_datalen;

  right2.data = &buf2;
  right2.mimeType = "video/mp4";
  right2.accountId = 0;
  right2.subscriptionId = 0;

  status_t ret;

  ret = DrmKernel_NvDrmPlugin_onSaveRights(1, &right1, "/no/path", contentId1);
  printf("===> Saving right1 for contentId1: %s (OK expected) => %s\n", ret == NV_NO_ERROR ? "OK" : "NOK", ret == NV_NO_ERROR ? "PASSED" : "FAILED");
  ret = DrmKernel_NvDrmPlugin_onSaveRights(2, &right2, "/no/path", contentId2);
  printf("===> Saving right2 for contentId2: %s (OK expected) => %s\n", ret == NV_NO_ERROR ? "OK" : "NOK", ret == NV_NO_ERROR ? "PASSED" : "FAILED");

  ret = DrmKernel_NvDrmPlugin_onSaveRights(3, &right1, "/no/path", contentId2);
  printf("===> Saving right1 for contentId2: %s (NOK expected) => %s\n", ret == NV_NO_ERROR ? "OK" : "NOK", ret != NV_NO_ERROR ? "PASSED" : "FAILED");
  ret = DrmKernel_NvDrmPlugin_onSaveRights(4, &right2, "/no/path", contentId1);
  printf("===> Saving right2 for contentId1: %s (NOK expected) => %s\n", ret == NV_NO_ERROR ? "OK" : "NOK", ret != NV_NO_ERROR ? "PASSED" : "FAILED");

  return(0);
}
