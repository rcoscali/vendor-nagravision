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

#ifndef __NV_CRYPTO_FACTORY_H__
#define __NV_CRYPTO_FACTORY_H__

#include <media/hardware/CryptoAPI.h>

#define NV_DRM_SCHEME_UUID 	"bcebe8eb-b0a2-4f71-9929-de2e0fcaf53a"
#define MSPR_DRM_SCHEME_UUID	"9a04f079-9840-4286-ab92-e65be0885f95"
#define DASHIF0_DRM_SCHEME_UUID	"00000000-0000-0000-0000-000000000000"
#define DASHIF1_DRM_SCHEME_UUID	"00000000-0000-0000-0000-000000000001"
#define DASHIF2_DRM_SCHEME_UUID	"00000000-0000-0000-0000-000000000002"

#ifndef SYM_EXPORT
#define SYM_EXPORT	__attribute__ ((visibility ("default")))
#endif

using namespace android;
extern "C" {
      SYM_EXPORT CryptoFactory *createCryptoFactory();
}

namespace android {

  class NvCryptoFactory : public CryptoFactory {
  public:
    NvCryptoFactory();
    virtual ~NvCryptoFactory();

    SYM_EXPORT bool isCryptoSchemeSupported(const uint8_t uuid[16]) const;

    SYM_EXPORT virtual status_t createPlugin(const uint8_t uuid[16], 
					     const void *data, 
					     size_t size,
					     CryptoPlugin **plugin);
  };

}

#endif /* __NV_CRYPTO_FACTORY_H__ */
