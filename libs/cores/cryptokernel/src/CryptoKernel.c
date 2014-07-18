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
  *errorDetailMsg = strdup("Not yet implemented !!");
  return 0;
}
