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
#include <string.h>
#include <strings.h>

#define LOG_NDEBUG 0
#define LOG_TAG "CryptoKernel"
#include <utils/Log.h>

#include <openssl/aes.h>

#include "CryptoKernel.h"

#define NVMIN(a, b)  ((a) < (b) ? (a) : (b))

/*
 * 
 */
#ifdef CRYPTO_HOST_KERNEL_TEST
#ifdef __cplusplus
extern "C"
#endif
static void printBuf(const char *msg, const uint8_t *buf, const size_t len)
{
  char outbuf[10240]; // To be large
  unsigned int i = 0;

  bzero(outbuf, 10240);
  for (i = 0; i < NVMIN(len, 10240); i++)
    {
      uint8_t v = buf[i];
      sprintf(&outbuf[i*2], "%02x ", v);
    }
  if (len > 10240/2)
    ALOGV("%s%s...(truncated)", msg, outbuf);
  else
    ALOGV("%s%s", msg, outbuf);
}
#else
#ifdef __cplusplus
extern "C"
#endif
static inline void printBuf(__attribute__((unused)) const char *msg, 
			    __attribute__((unused)) const uint8_t *buf, 
			    __attribute__((unused)) const size_t len)
{
  
}
#endif

/*
 * Crypto Kernel entry point for NvCryptoPlugin::requiresSecureDecoderComponent
 *
 * @param mime	mime type of the content
 * @return 1 (true) if secure decoder required, 0 (false) otherwise
 */
char
CryptoKernel_NvCryptoPlugin_requiresSecureDecoderComponent(const char *mime)
{
  if (!strncmp(mime, "video/", strlen("video/")))
    return (char)1;

  return (char)0;
}

extern int DrmKernel_getRightKey(const uint8_t *, uint8_t **, size_t *);

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
ssize_t CryptoKernel_NvCryptoPlugin_decrypt(__attribute__((unused)) char secure, 
					    const uint8_t keyid[16], 
					    const uint8_t iv[16], 
					    const void *srcPtr, 
					    const struct NV_SubSample_st *subSamples, 
					   __attribute__((unused))  size_t numSubSamples, 
					    void *dstPtr, 
					    char **errorDetailMsg)
{
  ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Enter");

  unsigned long dataSize = 0;  
  AES_KEY aesKey; // AA1: aes key schedules bufs for content key

  uint8_t *key; // AA1: unprotected content key tmp buffer
  size_t keylen = 0;

  // AA1: get the unprotected content key
  if (DrmKernel_getRightKey(keyid, &key, &keylen))
    {
      ALOGE("CryptoKernel_NvCryptoPlugin_decrypt - Invalid right or no key found for keyid");
      if (errorDetailMsg != NULL)
	*errorDetailMsg = "Invalid right";
    }
  else
    {
      // AA1: init aes rounds for key schedule
      if (AES_set_encrypt_key(key, AES_BLOCK_SIZE * 8, &aesKey) < 0) 
	{
	  bzero((void *)key, keylen);
	  (void)AES_set_encrypt_key(key, AES_BLOCK_SIZE * 8, &aesKey); // make rounds with zero'ed
	  free ((void *)key);
	  ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Unable to set decryption key");
	  if (errorDetailMsg != NULL)
	    *errorDetailMsg = "Invalid key";
	}
      
      else
	{
	  unsigned int num = 0;
	  unsigned char ecount_buf[AES_BLOCK_SIZE];
	  // should zero kc tmp buf here, but for displaying it we delay
	  //bzero((void *)key, keylen);
	  bzero(ecount_buf, AES_BLOCK_SIZE);
	  
	  dataSize = subSamples->mNumBytesOfEncryptedData 
	    + subSamples->mNumBytesOfClearData;
	  
	  ALOGI("===========================================================================");
	  ALOGI("AES_KEY: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		key[0],  key[1],  key[2],  key[3],  key[4],  key[5],  key[6],  key[7], 
		key[8],  key[9],  key[10], key[11], key[12], key[13], key[14], key[15]);
	  // now zero kc tmp buf
	  bzero((void *)key, keylen);
	  ALOGI("IV: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		iv[0],  iv[1],  iv[2],  iv[3],  iv[4],  iv[5],  iv[6],  iv[7], 
		iv[8],  iv[9],  iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]);
	  ALOGI("Num Bytes Of Clear Data: %d", subSamples->mNumBytesOfClearData);
	  ALOGI("Num Bytes Of Encrypted Data: %d", subSamples->mNumBytesOfEncryptedData);
	  ALOGI("Data Size: %lu", dataSize);
	  ALOGI("===========================================================================");
	  
	  if (dataSize == 0 || 
	      (subSamples->mNumBytesOfEncryptedData % AES_BLOCK_SIZE) != 0)
	    {
	      ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Wrong buffer data size >%lu - %d<"
		    " (empty or not multiple of 16 bytes)", 
		    dataSize, subSamples->mNumBytesOfEncryptedData);
	      if (errorDetailMsg != NULL)
		*errorDetailMsg = "Invalid size";
	      dataSize = 0;
	    }
	  
	  else
	    {
#ifdef DUMP_SUBSAMPLES
	      ALOGV(   ">>> Subsamples dump:");
	      ALOGV(   ">>>   mNumBytesOfClearData    : %d", subSamples->mNumBytesOfClearData);
	      ALOGV(   ">>>   mNumBytesOfEncryptedData: %d", subSamples->mNumBytesOfEncryptedData);
	      printBuf(">>>   ClearData               : ", srcPtr, subSamples->mNumBytesOfClearData);
	      printBuf(">>>   EncryptedData           : ", (uint8_t *)srcPtr + subSamples->mNumBytesOfClearData, subSamples->mNumBytesOfEncryptedData);
#endif		      
	      unsigned char* pInBuffer = (unsigned char*) srcPtr;
	      unsigned char* pOutBuffer = (unsigned char*) dstPtr;

	      if (subSamples->mNumBytesOfClearData > 0) 
		memcpy(pOutBuffer, pInBuffer, subSamples->mNumBytesOfClearData);
	      
	      if (subSamples->mNumBytesOfEncryptedData > 0)
		{
		  uint32_t i = 0;
		  for (i = subSamples->mNumBytesOfClearData;
		       i < dataSize; 
		       i += AES_BLOCK_SIZE) 
		    {
		      // AA1: use content key for descrambling with AES-CTR
		      AES_ctr128_encrypt(pInBuffer + i, pOutBuffer + i, 
					 AES_BLOCK_SIZE, (const AES_KEY *)&aesKey, 
					 (unsigned char *)iv, ecount_buf, &num);
		      /* Only display 1 block per 16 */
		      if (((i-subSamples->mNumBytesOfClearData)/16) %16 == 0)
			{
			  ALOGV("IV[%d]: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
				(i-subSamples->mNumBytesOfClearData)/16,
				iv[0],  iv[1],  iv[2],  iv[3],  iv[4],  iv[5],  iv[6],  iv[7], 
				iv[8],  iv[9],  iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]);
			  ALOGV("ecount_buf: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
				ecount_buf[0],  ecount_buf[1],  ecount_buf[2],  ecount_buf[3],  
				ecount_buf[4],  ecount_buf[5],  ecount_buf[6],  ecount_buf[7], 
				ecount_buf[8],  ecount_buf[9],  ecount_buf[10], ecount_buf[11], 
				ecount_buf[12], ecount_buf[13], ecount_buf[14], ecount_buf[15]);
			}
		    }
		  
		  *errorDetailMsg = "";
		}
	    }
	  // AA1: zero content key
	  (void)AES_set_encrypt_key(key, AES_BLOCK_SIZE * 8, &aesKey); // make rounds with zero'ed key
	  free ((void *)key);
	}
    }
  
  ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Exit");
  return dataSize;
}
