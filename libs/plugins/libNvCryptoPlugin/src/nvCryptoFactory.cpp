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

#define LOG_NDEBUG 0
#define LOG_TAG "nvCryptoFactory"
#include <utils/Log.h>

#include "nvCryptoFactory.h"
#include "uuid.h"

using namespace android;

/* ==========================================================================
 * Static definitions for factory
 * ==========================================================================*/

/* ==========================================================================
 * Implementation of the CryptoFactory class
 * ==========================================================================*/

// Shared library entry point
SYM_EXPORT CryptoFactory *
createCryptoFactory()
{
    return new NvCryptoFactory();
}

/*
 * Public Constructor
 */
NvCryptoFactory::NvCryptoFactory()
  : CryptoFactory()
{
  ALOGV("NvCryptoFactory::NvCryptoFactory() - Enter");
}

/*
 * Virtual Destructor
 */
NvCryptoFactory::~NvCryptoFactory() 
{
  ALOGV("NvCryptoFactory::~NvCryptoFactory()  - Enter");
}

/* ==========================================================================
 * Implementation of the CryptoFactory
 * ==========================================================================*/

#define TEST_UUID_MATCH(x)  					\
  if (uuid_str2bin((x), nvUuid) == NV_OK)               	\
    {								\
      if (!uuid_cmp(uuid, nvUuid)) 				\
        {							\
          ALOGI("*** Found a match for UUID %s!\n", (x));       \
          return true;						\
        }							\
    }

/*
 * NvCryptoFactory::isCryptoSchemeSupported
 */
SYM_EXPORT bool 
NvCryptoFactory::isCryptoSchemeSupported(const uint8_t uuid[16]) const
{
  ALOGV("NvCryptoFactory::isCryptoSchemeSupported() - Enter\n");
  ALOGV("NvCryptoFactory::isCryptoSchemeSupported - UUID %02x%02x%02x%02x-%02x%02x-%02x%02x-"
        "%02x%02x-%02x%02x%02x%02x%02x%02x\n",
        uuid[0], uuid[1], uuid[2], uuid[3], 
        uuid[4], uuid[5], uuid[6], uuid[7], 
        uuid[8], uuid[9], uuid[10], uuid[11], 
        uuid[12], uuid[13], uuid[14], uuid[15]);
  
  uint8_t nvUuid[16];

  /* Nagravision DRM scheme */
  TEST_UUID_MATCH(NV_DRM_SCHEME_UUID);
  /* Microsoft Play Ready DRM scheme */
  TEST_UUID_MATCH(MSPR_DRM_SCHEME_UUID);
  /* DASH IF test 0 DRM scheme */
  TEST_UUID_MATCH(DASHIF0_DRM_SCHEME_UUID);
  /* DASH IF test 1 DRM scheme */
  TEST_UUID_MATCH(DASHIF1_DRM_SCHEME_UUID);
  /* DASH IF test 2 DRM scheme */
  TEST_UUID_MATCH(DASHIF2_DRM_SCHEME_UUID);

  ALOGV("No match at all for UUID %02x%02x%02x%02x-%02x%02x-%02x%02x-"
        "%02x%02x-%02x%02x%02x%02x%02x%02x\n",
        uuid[0], uuid[1], uuid[2], uuid[3], 
        uuid[4], uuid[5], uuid[6], uuid[7], 
        uuid[8], uuid[9], uuid[10], uuid[11], 
        uuid[12], uuid[13], uuid[14], uuid[15]);
  return true;
}

/*
 * NvCryptoFactory::createPlugin
 */
extern "C" { extern CryptoPlugin* create(); }
SYM_EXPORT status_t 
NvCryptoFactory::createPlugin(const uint8_t        uuid[16], 
			      const void          *data, 
				    size_t         size,
				    CryptoPlugin **plugin)
{
  ALOGV("NvCryptoFactory::createPlugin() - Enter");
  if (plugin)
    {
      *plugin = create();
      return NO_ERROR;
    }
  return NO_MEMORY;
}
