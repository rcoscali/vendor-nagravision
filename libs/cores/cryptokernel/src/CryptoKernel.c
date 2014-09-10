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

#include <string.h>
#include <strings.h>

#define LOG_NDEBUG 0
#define LOG_TAG "CryptoKernel"
#include <utils/Log.h>

#include <openssl/aes.h>

#include "CryptoKernel.h"

/*
 * Crypto Kernel entry point for NvCryptoPlugin::requiresSecureDecoderComponent
 *
 * @param mime	mime type of the content
 * @return 1 (true) if secure decoder required, 0 (false) otherwise
 */
char CryptoKernel_NvCryptoPlugin_requiresSecureDecoderComponent(const char *mime)
{
  if (strncmp(mime, "application/vnd.nagra.drm", strlen("application/vnd.nagra.drm")))
    return (char)0;

  return (char)1;
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
ssize_t CryptoKernel_NvCryptoPlugin_decrypt(char secure, 
					    const uint8_t key[16], 
					    const uint8_t iv[16], 
					    const void *srcPtr, 
					    const struct NV_SubSample_st *subSamples, 
					    size_t numSubSamples, 
					    void *dstPtr, 
					    char **errorDetailMsg)
{
  ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Enter");

  unsigned long dataSize = 0;  
  AES_KEY openSSLKey;
  
  if (AES_set_encrypt_key(key, AES_BLOCK_SIZE * 8, &openSSLKey) < 0) 
    {
      ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Unable to set decryption key\n");
      if (errorDetailMsg != NULL)
        *errorDetailMsg = "Invalid key";
    }
  
  else
    {
      unsigned int num = 0;
      unsigned char ecount[AES_BLOCK_SIZE];

      dataSize = subSamples->mNumBytesOfEncryptedData 
	+ subSamples->mNumBytesOfClearData;
	  
      ALOGI("===========================================================================\n");
      ALOGI("AES_KEY: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	    key[0],  key[1],  key[2],  key[3],  key[4],  key[5],  key[6],  key[7], 
	    key[8],  key[9],  key[10], key[11], key[12], key[13], key[14], key[15]);
      ALOGI("Num Bytes Of Clear Data: %d\n", subSamples->mNumBytesOfClearData);
      ALOGI("Num Bytes Of Encrypted Data: %d\n", subSamples->mNumBytesOfEncryptedData);
      ALOGI("Data Size: %lu\n", dataSize);
      ALOGI("===========================================================================\n");

      bzero(ecount, AES_BLOCK_SIZE);
      if (dataSize == 0 || 
	  (subSamples->mNumBytesOfEncryptedData % AES_BLOCK_SIZE) != 0 ||
          subSamples->mNumBytesOfEncryptedData == 0)
        {
          ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Wrong buffer data size >%lu - %d<"
                " (empty or not multiple of 16 bytes)\n", 
                dataSize, subSamples->mNumBytesOfEncryptedData);
          if (errorDetailMsg != NULL)
            *errorDetailMsg = "Invalid size";
          dataSize = 0;
        }
	  
      else
	{
	  unsigned char* pInBuffer = (unsigned char*) srcPtr;
	  unsigned char* pOutBuffer = (unsigned char*) dstPtr;
	  
	  if (subSamples->mNumBytesOfClearData > 0) 
	    memcpy(pOutBuffer, pInBuffer, subSamples->mNumBytesOfClearData);
	  
          uint32_t i = 0;
          for (i = subSamples->mNumBytesOfClearData;
               i < dataSize; 
               i += AES_BLOCK_SIZE) 
            AES_ctr128_encrypt(pInBuffer + i, pOutBuffer + i, 
                               AES_BLOCK_SIZE, (const AES_KEY *)&openSSLKey, 
                               (unsigned char *)iv, ecount, &num);
	}
    }

  ALOGV("CryptoKernel_NvCryptoPlugin_decrypt - Exit");
  return dataSize;
}
