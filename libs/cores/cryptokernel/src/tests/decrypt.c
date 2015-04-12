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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
/* 
   For generating a protected content right key, 
   key being: [0xB7, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13, 0xA0, 0xB1, 0xC2, 0xD3, 0xE0, 0xF1, 0xE2, 0xF3]

       cohen@PC12316-LX:~$ echo -ne "\xb7\x01\x02\x03\x10\x11\x12\x13\xa0\xb1\xc2\xd3\xe0\xf1\xe2\xf3" | \
       > openssl enc -e -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | \
       > od -A n -w32 -t x1
        4a fe a1 d7 a5 e7 48 c7 a6 fa 4d 0a 81 33 1f 7e d9 ba 01 45 4b d6 29 ca ac 37 4a e7 7d 66 85 91
       cohen@PC12316-LX:~$

for generating a content right tag:
   # echo -ne "contentId + keyId + <length> + <protectedKey>" | openssl sha256 -binary | openssl enc -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | od -A n -w32 -t x1
   ex.:

   Key and IV are the one defined in DrmKernel.c provided to openssl as hex bytes string

   For the right 1 test vector (note the \x00 at end of content id, it is a C string: need it at right decoding)
     "Android.mk"                \x41\x6e\x64\x72\x6f\x69\x64\x2e\x6d\x6b\x00
     keyId:                      \x12\x1a\x0f\xca\x0f\x1b\x47\x5b\x89\x10\x29\x7f\xa8\xe0\xa0\x7e
     Length                      \x00\x20
     Protected Key #1            \x4a\xfe\xa1\xd7\xa5\xe7\x48\xc7\xa6\xfa\x4d\x0a\x81\x33\x1f\x7e\xd9\xba\x01\x45\x4b\xd6\x29\xca\xac\x37\x4a\xe7\x7d\x66\x85\x91

       cohen@PC12316-LX:~$ echo -ne "\x41\x6e\x64\x72\x6f\x69\x64\x2e\x6d\x6b\x00\
       > \x12\x1a\x0f\xca\x0f\x1b\x47\x5b\x89\x10\x29\x7f\xa8\xe0\xa0\x7e\x00\x20\
       > \x4a\xfe\xa1\xd7\xa5\xe7\x48\xc7\xa6\xfa\x4d\x0a\x81\x33\x1f\x7e\xd9\xba\x01\x45\x4b\xd6\x29\xca\xac\x37\x4a\xe7\x7d\x66\x85\x91" | \
       > openssl sha256 -binary | openssl enc -e -nopad -aes-128-cbc -K B701020310111213A0B1C2D3E0F1E2F3 -iv a9323031356e61677261766973696f6e | \
       > od -A n -w32 -t x1
        e1 e1 7d 7e 00 dc a6 99 64 36 31 c3 ae 17 d4 8a 3a 92 f8 ca 75 43 e4 5f 09 29 2b aa 48 e0 c4 5f
       cohen@PC12316-LX:~$ 

 */

#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>

#include <openssl/aes.h>
#include <openssl/sha.h>

#ifdef CRYPTO_HOST_KERNEL_TEST
typedef __off64_t off64_t;
#endif

#include "DrmKernel.h"
#include "CryptoKernel.h"

/* test vectors: a payload ciphered as isobmff cenc */
#include "decrypt_data.h"

#define NVMIN(a, b)  ((a) < (b) ? (a) : (b))

const char secure = 0;
uint8_t keyid[AES_BLOCK_SIZE] = {
  0x12, 0x1a, 0x0f, 0xca, 0x0f, 0x1b, 0x47, 0x5b, 0x89, 0x10, 0x29, 0x7f, 0xa8, 0xe0, 0xa0, 0x7e
};
uint8_t iv1[AES_BLOCK_SIZE] = {
  0xa9, 0x32, 0x30, 0x31, 0x35, 0x6e, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
};
uint8_t iv2[AES_BLOCK_SIZE] = {
  0xa9, 0x32, 0x30, 0x31, 0x35, 0x6e, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
};

void *dstPtr = NULL;

const uint8_t right1_right_data[] = {
  0x41, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x2e, 0x6d, 0x6b, 0x00,
  0x12, 0x1a, 0x0f, 0xca, 0x0f, 0x1b, 0x47, 0x5b, 0x89, 0x10, 0x29, 0x7f, 0xa8, 0xe0, 0xa0, 0x7e,
  0x00, 0x20,
  0x4a, 0xfe, 0xa1, 0xd7, 0xa5, 0xe7, 0x48, 0xc7, 0xa6, 0xfa, 0x4d, 0x0a, 0x81, 0x33, 0x1f, 0x7e, 0xd9, 0xba, 0x01, 0x45, 0x4b, 0xd6, 0x29, 0xca, 0xac, 0x37, 0x4a, 0xe7, 0x7d, 0x66, 0x85, 0x91,
  0xe1, 0xe1, 0x7d, 0x7e, 0x00, 0xdc, 0xa6, 0x99, 0x64, 0x36, 0x31, 0xc3, 0xae, 0x17, 0xd4, 0x8a, 0x3a, 0x92, 0xf8, 0xca, 0x75, 0x43, 0xe4, 0x5f, 0x09, 0x29, 0x2b, 0xaa, 0x48, 0xe0, 0xc4, 0x5f
};
const int right1_right_datalen = 93;

#ifdef CRYPTO_HOST_KERNEL_TEST
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
#endif

char *bintobuf(char *outbuf, const uint8_t *buf, size_t len)
{
  unsigned int i = 0;

  bzero(outbuf, len*2);
  for (i = 0; i < len; i++)
    {
      uint8_t v = buf[i];
      sprintf(&outbuf[i*2], "%02x ", v);
    }
  return outbuf;
}


/*
 * Crypto Kernel entry point for NvCryptoPlugin::decrypt
 *
 * @param secure	secure decoder required or not
 * @param key		AES-128-CTR key to use for deciphering
 * @param iv		AES-128-CTR iv to use for deciphering
 * @param srcPtr	source pointer to input sample ciphered data
 * @param subSamples	sub-samples array (according to CENC) containing 
 * 			ciphered/unciphered AVC data
 * @param numSubSamples	number of sub-samples in this sample
 * @param dstPtr	destination pointer to output sample deciphered data
 * @param errorMsg	pointer of pointer to string used as a return parameter
 *			for providing a detailled error message
 */
/*
ssize_t CryptoKernel_NvCryptoPlugin_decrypt(__attribute__((unused)) char secure, 
					    const uint8_t keyid[16], 
					    const uint8_t iv[16], 
					    const void *srcPtr, 
					    const struct NV_SubSample_st *subSamples, 
					   __attribute__((unused))  size_t numSubSamples, 
					    void *dstPtr, 
					    char **errorDetailMsg)
*/

int main()
{
  int retall = 0;
  char *errMsg = NULL;
  char outbuf[1024];

  const char *contentId1 = "Android.mk";

  struct NV_DrmBuffer_st buf1;
  struct NV_DrmRights_st right1;

  buf1.data = (char *)right1_right_data;
  buf1.length = right1_right_datalen;

  right1.data = &buf1;
  right1.mimeType = "text/ascii";
  right1.accountId = 0;
  right1.subscriptionId = 0;

  status_t ret;

  DrmKernel_init();

  ret = DrmKernel_NvDrmPlugin_onSaveRights(1, &right1, "/no/path", contentId1);
  printf("===> Saving right1 for contentId1: %s (OK expected) => %s\n", ret == NV_NO_ERROR ? "OK" : "NOK", ret == NV_NO_ERROR ? "PASSED" : "FAILED");
  retall = ret == NV_NO_ERROR;

  dstPtr = (void *)malloc(bufptr1_datasize +bufptr2_datasize +1);
  bzero(dstPtr, bufptr1_datasize +bufptr2_datasize +1);

  /* In this test we substitue ourself with the android */
  /* media framework via a direct call to decrypt */

  CryptoKernel_NvCryptoPlugin_decrypt (secure,
  				       keyid,
  				       iv1,
  				       bufptr1,
  				       &bufptr1_subsamples,
  				       1,
  				       dstPtr,
  				       &errMsg);
  CryptoKernel_NvCryptoPlugin_decrypt (secure,
				       keyid,
				       iv2,
				       bufptr2,
				       &bufptr2_subsamples,
				       1,
				       (uint8_t *)dstPtr+bufptr1_datasize,
				       &errMsg);

  ret = memcmp(origbuf, dstPtr, bufptr1_datasize +bufptr2_datasize);
  printf("===> Decrypt Cenc : %s (OK expected) => %s\n", ret == NV_NO_ERROR ? "OK" : "NOK", ret == NV_NO_ERROR ? "PASSED" : "FAILED");
  retall &= ret == NV_NO_ERROR;

  free (dstPtr);

  return(retall);
}
