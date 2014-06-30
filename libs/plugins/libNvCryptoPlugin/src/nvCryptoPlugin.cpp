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
#define LOG_TAG "nvCryptoPlugin"
#include <utils/Log.h>

#include "nvCryptoPlugin.h"

using namespace android;

/* ==========================================================================
 * Static definitions for plugin
 * ==========================================================================*/

static NvCryptoPlugin *sNvCryptoPluginInstance = (NvCryptoPlugin *)NULL;

/*
 * create
 *
 * This extern "C" is mandatory to be managed by TPlugInManager
 */
extern "C" CryptoPlugin* 
create() 
{
  ALOGV("CryptoPlugin* create() - Enter");

  if ((NvCryptoPlugin *)NULL == sNvCryptoPluginInstance)
    {
      sNvCryptoPluginInstance = new NvCryptoPlugin();
    }

  ALOGV("CryptoPlugin* create() - Exit : instance=0x%p", sNvCryptoPluginInstance);
  return (CryptoPlugin*)sNvCryptoPluginInstance;
}

/*
 * destroy
 *
 * This extern "C" is mandatory to be managed by TPlugInManager
 */
extern "C" void 
destroy(CryptoPlugin* pPlugIn) 
{
  ALOGV("void destroy(CryptoPlugin* pPlugIn) - Enter : 0x%p", pPlugIn);

  if (pPlugIn == (CryptoPlugin *)sNvCryptoPluginInstance)
    {
      delete sNvCryptoPluginInstance;
      sNvCryptoPluginInstance = (NvCryptoPlugin *)NULL;
    }
  else
    {
      delete pPlugIn;
    }
  pPlugIn = (CryptoPlugin*)NULL;

  ALOGV("void destroy(CryptoPlugin* pPlugIn) - Exit");
}


/* ==========================================================================
 * Implementation of the NvCryptoPlugin class
 * ==========================================================================*/

/*
 * Public Constructor
 */
NvCryptoPlugin::NvCryptoPlugin()
  : CryptoPlugin() 
{
  ALOGV("NvCryptoPlugin::NvCryptoPlugin() - Enter");
}

/* 
 * Virtual Public Destructor
 */
NvCryptoPlugin::~NvCryptoPlugin()
{
  ALOGV("NvCryptoPlugin::~NvCryptoPlugin() - Enter");
}

/* ==========================================================================
 * Implementation of the CryptoPlugin
 * ==========================================================================*/

/*
 * NvCryptoPlugin::requiresSecureDecoderComponent
 */
bool 
NvCryptoPlugin::requiresSecureDecoderComponent(const char *mime) const
{
  ALOGV("NvCryptoPlugin::requiresSecureDecoderComponent() - Enter");

  return strncmp(mime, "application/vnd.nagra.drm", strlen("application/vnd.nagra.drm"));
}

/*
 * NvCryptoPlugin::decrypt
 */
ssize_t 
NvCryptoPlugin::decrypt(      bool       secure,
			const uint8_t    key[16],
			const uint8_t    iv[16],
			      Mode       mode,
			const void      *srcPtr,
			const SubSample *subSamples, 
			      size_t     numSubSamples,
			      void      *dstPtr,
			      AString   *errorDetailMsg)
{
  ALOGV("NvCryptoPlugin::decrypt() - Enter");

  return 0;
}
