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

#include <media/stagefright/foundation/AString.h>

#define LOG_NDEBUG 0
#define LOG_TAG "nvCryptoPlugin"
#include <utils/Log.h>

#include "String8.h"
#include "drm/DrmAPI.h"
#include "nvCryptoPlugin.h"
#include "media/stagefright/MediaErrors.h"
#include "CryptoKernel.h"

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
extern "C" 
SYM_EXPORT CryptoPlugin* 
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
extern "C" 
SYM_EXPORT void 
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
SYM_EXPORT bool 
NvCryptoPlugin::requiresSecureDecoderComponent(const char *mime) const
{
  ALOGV("NvCryptoPlugin::requiresSecureDecoderComponent() - Enter");

  return static_cast<bool>(CryptoKernel_NvCryptoPlugin_requiresSecureDecoderComponent(mime));
}

/*
 * Conversion utilities
 */
// NvCryptoPlugin::arrayToString
String8 NvCryptoPlugin::arrayToString(uint8_t const *array, size_t len) const
{
  String8 result("{ ");
  for (size_t i = 0; i < len; i++) {
    result.appendFormat("0x%02x ", array[i]);
  }
  result += "}";
  return result;
}

// NvCryptoPlugin::subSamplesToString
String8 NvCryptoPlugin::subSamplesToString(SubSample const *subSamples,
					     size_t numSubSamples) const
{
  String8 result;
  for (size_t i = 0; i < numSubSamples; i++) {
    result.appendFormat("[%d] {clear:%d, encrypted:%d} ", i,
			subSamples[i].mNumBytesOfClearData,
			subSamples[i].mNumBytesOfEncryptedData);
  }
  return result;
}

/*
 * NvCryptoPlugin::decrypt
 */
SYM_EXPORT ssize_t 
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

  char *localErrorDetailMsg = NULL;
  if (mode != kMode_AES_CTR) {
    if (errorDetailMsg != (AString *)NULL)
      {
	AString localAStringErrorDetailMsg("Invalid encryption mode - Only support AES_CTR !");
	*errorDetailMsg = localAStringErrorDetailMsg;
      }
    return 0;
  }
  struct NV_SubSample_st localSubSamples;
  localSubSamples.mNumBytesOfClearData     = subSamples->mNumBytesOfClearData;
  localSubSamples.mNumBytesOfEncryptedData = subSamples->mNumBytesOfEncryptedData;
  ssize_t ret = CryptoKernel_NvCryptoPlugin_decrypt(static_cast<char>(secure),
						    key, iv,
						    srcPtr,
						    &localSubSamples, numSubSamples,
						    dstPtr,
						    &localErrorDetailMsg);
  if (errorDetailMsg != (AString *)NULL)
    {
      AString localAStringErrorDetailMsg(localErrorDetailMsg);
      *errorDetailMsg = localAStringErrorDetailMsg;
    }

  return ret;
}
