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

#ifndef __NV_CRYPTO_PLUGIN_H__
#define __NV_CRYPTO_PLUGIN_H__

#include <media/ICrypto.h>

#ifndef SYM_EXPORT
#define SYM_EXPORT	__attribute__ ((visibility ("default")))
#endif

namespace android {

  class NvCryptoPlugin : public CryptoPlugin {

  public:
    NvCryptoPlugin();
    virtual ~NvCryptoPlugin();

  protected:
    bool requiresSecureDecoderComponent(const char *mime) const;

    virtual ssize_t decrypt(bool secure,
			    const uint8_t key[16],
			    const uint8_t iv[16],
			    Mode mode,
			    const void *srcPtr,
			    const SubSample *subSamples, size_t numSubSamples,
			    void *dstPtr,
			    AString *errorDetailMsg);


  };

};

#endif /* __NV_CRYPTO_PLUGIN_H__ */

