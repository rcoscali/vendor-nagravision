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

/*
 * NvCryptoFactory::isCryptoSchemeSupported
 */
SYM_EXPORT bool 
NvCryptoFactory::isCryptoSchemeSupported(const uint8_t uuid[16]) const
{
  ALOGV("NvCryptoFactory::isCryptoSchemeSupported() - Enter : uuid='%s'", uuid);

  uint8_t nvUuid[16];

  /* Nagravision DRM scheme */
  if (uuid_str2bin(NV_DRM_SCHEME_UUID, nvUuid))
      if (!uuid_cmp(uuid, nvUuid)) return true;

  /* Microsoft Play Ready DRM scheme */
  if (uuid_str2bin(MSPR_DRM_SCHEME_UUID, nvUuid))
      if (!uuid_cmp(uuid, nvUuid)) return true;

  /* DASH IF test 0 DRM scheme */
  if (uuid_str2bin(DASHIF0_DRM_SCHEME_UUID, nvUuid))
      if (!uuid_cmp(uuid, nvUuid)) return true;

  /* DASH IF test 1 DRM scheme */
  if (uuid_str2bin(DASHIF1_DRM_SCHEME_UUID, nvUuid))
      if (!uuid_cmp(uuid, nvUuid)) return true;

  /* DASH IF test 2 DRM scheme */
  if (uuid_str2bin(DASHIF2_DRM_SCHEME_UUID, nvUuid))
      if (!uuid_cmp(uuid, nvUuid)) return true;

  return false;
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
