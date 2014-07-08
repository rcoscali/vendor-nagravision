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

#ifndef __CRYPTO_KERNEL_H__
#define __CRYPTO_KERNEL_H__

#ifdef __cplusplus
extern "C" {
#endif

char CryptoKernel_NvCryptoPlugin_requiresSecureDecoderComponent(const char *mime);
ssize_t CryptoKernel_NvCryptoPlugin_decrypt(char secure, 
					      const uint8_t key[16], 
					      const uint8_t iv[16], 
					      const void *srcPtr, 
					      const SubSample *subSamples, 
					      size_t numSubSamples, 
					      void *dstPtr, 
					      const char **errorDetailMsg);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_KERNEL_H__ */
