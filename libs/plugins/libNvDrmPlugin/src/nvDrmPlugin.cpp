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

//#define LOG_NDEBUG 0
#define LOG_TAG "nvDrmPlugin"
#include <utils/Log.h>

#include <drm/DrmRights.h>
#include <drm/DrmConstraints.h>
#include <drm/DrmMetadata.h>
#include <drm/DrmInfo.h>
#include <drm/DrmInfoEvent.h>
#include <drm/DrmInfoStatus.h>
#include <drm/DrmConvertedStatus.h>
#include <drm/DrmInfoRequest.h>
#include <drm/DrmSupportInfo.h>
#include "nvDrmPlugin.h"

using namespace android;

/* ==========================================================================
 * Static definitions for plugin
 * ==========================================================================*/

static NvDrmPlugin *sNvDrmPluginInstance = (NvDrmPlugin *)NULL;

#define N_METADATA 2
static char const *sNvMetadata[N_METADATA*2] = {
  "Copyright", "NagraVision ¢ 2014",
  "Author", "Rémi Cohen-Scali"
};


/*
 * create
 *
 * This extern "C" is mandatory to be managed by TPlugInManager
 */
extern "C" IDrmEngine* create() 
{
  ALOGV("IDrmEngine* create() - Enter");

  if ((NvDrmPlugin *)NULL == sNvDrmPluginInstance)
    {
      sNvDrmPluginInstance = new NvDrmPlugin();
    }

  ALOGV("IDrmEngine* create() - Exit : instance=0x%p", sNvDrmPluginInstance);
  return (IDrmEngine*)sNvDrmPluginInstance;
}

/*
 * destroy
 *
 * This extern "C" is mandatory to be managed by TPlugInManager
 */
extern "C" void destroy(IDrmEngine* pPlugIn) 
{
  ALOGV("void destroy(IDrmEngine* pPlugIn) - Enter : 0x%p", pPlugIn);  

  if (pPlugIn == (IDrmEngine *)sNvDrmPluginInstance)
    {
      delete sNvDrmPluginInstance;
      sNvDrmPluginInstance = (NvDrmPlugin *)NULL;
    }
  else
    {
      delete pPlugIn;
    }
  pPlugIn = (IDrmEngine *)NULL;

  ALOGV("void destroy(IDrmEngine* pPlugIn) - Exit");  
}


/* ==========================================================================
 * Implementation of the DrmEngine class
 * ==========================================================================*/

/*
 * Public Constructor
 */
NvDrmPlugin::NvDrmPlugin()
  : DrmEngineBase() {
  ALOGV("NvDrmPlugin::NvDrmPlugin() - Enter");
  mNvDrmMetadata = new DrmMetadata();
  
  for (int n = 0; 
       n < N_METADATA/2; 
       n++)
    {
      String8 *key = new String8(sNvMetadata[2*n]);
      String8 *value = new String8(sNvMetadata[2*n+1]);
      mNvDrmMetadata->put(key, (char const *)value);

      ALOGV("NvDrmPlugin::NvDrmPlugin(): added metadata key='%s' value='%s'", key, value);

      delete key;
      delete value;
    }
}

/* 
 * Virtual Public Destructor
 */
NvDrmPlugin::~NvDrmPlugin() {
  ALOGV("NvDrmPlugin::~NvDrmPlugin() - Enter");
}

/* ==========================================================================
 * Implementation of the DrmEngine
 * ==========================================================================*/

/*
 * NvDrmPlugin::onGetMetadata
 */
DrmMetadata* NvDrmPlugin::onGetMetadata(int uniqueId, const String8* path) {
  ALOGV("NvDrmPlugin::onGetMetadata() - Enter : %d", uniqueId);
  return ((NULL != path) ? mNvDrmMetadata : (DrmMetadata*)NULL);
}

/*
 * NvDrmPlugin::onGetConstraints
 */
DrmConstraints* NvDrmPlugin::onGetConstraints(int uniqueId, 
					      const String8* path, 
					      int action)
{
  ALOGV("NvDrmPlugin::onGetConstraints() - Enter : %d", uniqueId);
  DrmConstraints* drmConstraints = new DrmConstraints();
  
  String8 value("dummy_available_time");
  char* charValue = NULL;
  charValue = new char[value.length() + 1];
  strncpy(charValue, value.string(), value.length());

  //Just add dummy available time for verification
  drmConstraints->put(&(DrmConstraints::LICENSE_AVAILABLE_TIME), charValue);

  return drmConstraints;
}

/*
 * NvDrmPlugin::onProcessDrmInfo
 */
DrmInfoStatus* NvDrmPlugin::onProcessDrmInfo(int uniqueId, const DrmInfo* drmInfo)
{
  ALOGV("NvDrmPlugin::onProcessDrmInfo() - Enter : %d", uniqueId);

  DrmInfoStatus* drmInfoStatus = NULL;
  if (NULL != drmInfo)
    {
      switch (drmInfo->getInfoType())
	{
	case DrmInfoRequest::TYPE_REGISTRATION_INFO: 
	  {
	    const DrmBuffer* emptyBuffer = new DrmBuffer();
	    drmInfoStatus = new DrmInfoStatus(DrmInfoStatus::STATUS_OK,
					      DrmInfoRequest::TYPE_REGISTRATION_INFO, 
					      emptyBuffer, 
					      drmInfo->getMimeType());
	  }
	  break;

	case DrmInfoRequest::TYPE_UNREGISTRATION_INFO:
	  {
	    const DrmBuffer* emptyBuffer = new DrmBuffer();
	    drmInfoStatus = new DrmInfoStatus(DrmInfoStatus::STATUS_OK,
					      DrmInfoRequest::TYPE_UNREGISTRATION_INFO, 
					      emptyBuffer, 
					      drmInfo->getMimeType());
	  }
	  break;
	
	case DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_INFO:
	  {
	    String8 licenseString("dummy_license_string");
	    const int bufferSize = licenseString.size();
	    char* data = NULL;
	    data = new char[bufferSize];
	    memcpy(data, licenseString.string(), bufferSize);
	    const DrmBuffer* buffer = new DrmBuffer(data, bufferSize);
	    drmInfoStatus = new DrmInfoStatus(DrmInfoStatus::STATUS_OK,
					      DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_INFO, 
					      buffer, 
					      drmInfo->getMimeType());
	  }
	  break;
	  
	}
    }

  ALOGV("NvDrmPlugin::onProcessDrmInfo() - Exit");
  return drmInfoStatus;
}

/*
 * NvDrmPlugin::onSetOnInfoListener
 */
status_t NvDrmPlugin::onSetOnInfoListener(int uniqueId, 
					  const IDrmEngine::OnInfoListener* infoListener)
{
  ALOGV("NvDrmPlugin::onSetOnInfoListener() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onInitialize
 */
status_t NvDrmPlugin::onInitialize(int uniqueId)
{
  ALOGV("NvDrmPlugin::onInitialize() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onTerminate
 */
status_t NvDrmPlugin::onTerminate(int uniqueId)
{
  ALOGV("NvDrmPlugin::onTerminate() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onGetSupportInfo
 */
DrmSupportInfo* NvDrmPlugin::onGetSupportInfo(int uniqueId)
{
  ALOGV("NvDrmPlugin::onGetSupportInfo() - Enter : %d", uniqueId);
  DrmSupportInfo* drmSupportInfo = new DrmSupportInfo();
  // Add mimetype's
  drmSupportInfo->addMimeType(String8("application/vnd.nagra.drm"));
  // Add File Suffixes
  drmSupportInfo->addFileSuffix(String8(".nvv"));
  // Add plug-in description
  drmSupportInfo->setDescription(String8("NagraVision plug-in"));

  ALOGV("NvDrmPlugin::onGetSupportInfo() : %d", uniqueId);
  return drmSupportInfo;
}

/*
 * NvDrmPlugin::onSaveRights
 */
status_t NvDrmPlugin::onSaveRights(int uniqueId, 
				   const DrmRights& drmRights,
				   const String8& rightsPath, 
				   const String8& contentPath)
{
  ALOGV("NvDrmPlugin::onSaveRights() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onAcquireDrmInfo
 */
DrmInfo* NvDrmPlugin::onAcquireDrmInfo(int uniqueId, 
				       const DrmInfoRequest* drmInfoRequest)
{
  ALOGV("NvDrmPlugin::onAcquireDrmInfo() - Enter : %d", uniqueId);
  DrmInfo* drmInfo = NULL;

  if (NULL != drmInfoRequest)
    {
      String8 dataString("dummy_acquistion_string");
      int length = dataString.length();
      char* data = NULL;
      data = new char[length];
      memcpy(data, dataString.string(), length);
      drmInfo = new DrmInfo(drmInfoRequest->getInfoType(),
			    DrmBuffer(data, length), 
			    drmInfoRequest->getMimeType());
    }

  ALOGV("NvDrmPlugin::onAcquireDrmInfo() - Exit");
  return drmInfo;
}

/*
 * NvDrmPlugin::onCanHandle
 */
bool NvDrmPlugin::onCanHandle(int uniqueId, const String8& path) 
{
  ALOGV("NvDrmPlugin::canHandle() - Enter : %d path='%s'", uniqueId, path.string());
  String8 extension = path.getPathExtension();
  extension.toLower();
  return (String8(".nvv") == extension);
}

/*
 * NvDrmPlugin::onGetOriginalMimeType
 */
String8 NvDrmPlugin::onGetOriginalMimeType(int uniqueId, 
					   const String8& path, 
					   int fd) 
{
  ALOGV("NvDrmPlugin::onGetOriginalMimeType() - Enter : %d path='%s'", uniqueId, path.string());
  return String8("video/mp4");
}

/*
 * NvDrmPlugin::onGetDrmObjectType
 */
int NvDrmPlugin::onGetDrmObjectType(int uniqueId, 
				    const String8& path, 
				    const String8& mimeType) 
{
  ALOGV("NvDrmPlugin::onGetDrmObjectType() - Enter : %d", uniqueId);
  return DrmObjectType::UNKNOWN;
}

/*
 * NvDrmPlugin::onCheckRightsStatus
 */
int NvDrmPlugin::onCheckRightsStatus(int uniqueId, const String8& path, int action) {

  ALOGV("NvDrmPlugin::onCheckRightsStatus() - Enter : %d", uniqueId);

  int rightsStatus = RightsStatus::RIGHTS_VALID;
  return rightsStatus;
}

/*
 * NvDrmPlugin::onConsumeRights
 */
status_t NvDrmPlugin::onConsumeRights(int uniqueId, 
				      DecryptHandle* decryptHandle,
				      int action, 
				      bool reserve) 
{
  ALOGV("NvDrmPlugin::onConsumeRights() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onSetPlaybackStatus
 */
status_t NvDrmPlugin::onSetPlaybackStatus(int uniqueId, 
					  DecryptHandle* decryptHandle,
					  int playbackStatus, 
					  int64_t position) 
{
  ALOGV("NvDrmPlugin::onSetPlaybackStatus() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onValidateAction
 */
bool NvDrmPlugin::onValidateAction(int uniqueId, 
				   const String8& path,
				   int action, 
				   const ActionDescription& description) 
{
  ALOGV("NvDrmPlugin::onValidateAction() - Enter : %d", uniqueId);
  return true;
}

/*
 * NvDrmPlugin::onRemoveRights
 */
status_t NvDrmPlugin::onRemoveRights(int uniqueId, 
				     const String8& path) 
{
  ALOGV("NvDrmPlugin::onRemoveRights() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onRemoveAllRights
 */
status_t NvDrmPlugin::onRemoveAllRights(int uniqueId) 
{
  ALOGV("NvDrmPlugin::onRemoveAllRights() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onOpenConvertSession
 */
status_t NvDrmPlugin::onOpenConvertSession(int uniqueId, 
					   int convertId) 
{
  ALOGV("NvDrmPlugin::onOpenConvertSession() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onConvertData
 */
DrmConvertedStatus* NvDrmPlugin::onConvertData(int uniqueId, 
					       int convertId, 
					       const DrmBuffer* inputData)
{
  ALOGV("NvDrmPlugin::onConvertData() - Enter : %d", uniqueId);
  DrmBuffer* convertedData = NULL;

  if (NULL != inputData && 0 < inputData->length)
    {
    int length = inputData->length;
    char* data = NULL;
    data = new char[length];
    convertedData = new DrmBuffer(data, length);
    memcpy(convertedData->data, inputData->data, length);
  }
  return new DrmConvertedStatus(DrmConvertedStatus::STATUS_OK, convertedData, 0 /*offset*/);
}

/*
 * NvDrmPlugin::onCloseConvertSession
 */
DrmConvertedStatus* NvDrmPlugin::onCloseConvertSession(int uniqueId, 
						       int convertId) 
{
  ALOGV("NvDrmPlugin::onCloseConvertSession() - Enter : %d", uniqueId);
  return new DrmConvertedStatus(DrmConvertedStatus::STATUS_OK, NULL, 0 /*offset*/);
}

/*
 * NvDrmPlugin::onOpenDecryptSession
 */
status_t NvDrmPlugin::onOpenDecryptSession(int uniqueId, 
					   DecryptHandle* decryptHandle, 
					   int fd, 
					   off64_t offset, 
					   off64_t length) 
{
  ALOGV("NvDrmPlugin::onOpenDecryptSession() - Enter : %d", uniqueId);

#ifdef ENABLE_PASSTHRU_DECRYPTION
  decryptHandle->mimeType = String8("video/passthru");
  decryptHandle->decryptApiType = DecryptApiType::ELEMENTARY_STREAM_BASED;
  decryptHandle->status = DRM_NO_ERROR;
  decryptHandle->decryptInfo = NULL;
  return DRM_NO_ERROR;
#endif

  return DRM_ERROR_CANNOT_HANDLE;
}

/*
 * NvDrmPlugin::onOpenDecryptSession
 */
status_t NvDrmPlugin::onOpenDecryptSession(int uniqueId, 
					   DecryptHandle* decryptHandle, 
					   const char* uri) 
{
  return DRM_ERROR_CANNOT_HANDLE;
}

/*
 * NvDrmPlugin::onCloseDecryptSession
 */
status_t NvDrmPlugin::onCloseDecryptSession(int uniqueId, 
					    DecryptHandle* decryptHandle) {
  ALOGV("NvDrmPlugin::onCloseDecryptSession() - Enter : %d", uniqueId);

  if (NULL != decryptHandle) 
    {
      if (NULL != decryptHandle->decryptInfo)
	{
	  delete decryptHandle->decryptInfo; 
	  decryptHandle->decryptInfo = NULL;
	}

      delete decryptHandle; decryptHandle = NULL;
    }

  ALOGV("NvDrmPlugin::onCloseDecryptSession() - Exit");
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onInitializeDecryptUnit
 */
status_t NvDrmPlugin::onInitializeDecryptUnit(int uniqueId, 
					      DecryptHandle* decryptHandle,
					      int decryptUnitId, 
					      const DrmBuffer* headerInfo) {
  ALOGV("NvDrmPlugin::onInitializeDecryptUnit() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onDecrypt
 */
status_t NvDrmPlugin::onDecrypt(int uniqueId, 
				DecryptHandle* decryptHandle,
				int decryptUnitId, 
				const DrmBuffer* encBuffer, 
				DrmBuffer** decBuffer, 
				DrmBuffer* IV) 
{
  ALOGV("NvDrmPlugin::onDecrypt() - Enter : %d", uniqueId);

  /**
   * As a workaround implementation passthru would copy the given
   * encrypted buffer as it is to decrypted buffer. Note, decBuffer
   * memory has to be allocated by the caller.
   */
  if (NULL != (*decBuffer) && 0 < (*decBuffer)->length)
    {
      if ((*decBuffer)->length >= encBuffer->length)
	{
	  memcpy((*decBuffer)->data, encBuffer->data, encBuffer->length);
	  (*decBuffer)->length = encBuffer->length;
	} 
      else 
	{
	  ALOGE("decBuffer size (%d) too small to hold %d bytes",
		(*decBuffer)->length, 
		encBuffer->length);
	  return DRM_ERROR_UNKNOWN;
	}
    }

  ALOGV("NvDrmPlugin::onDecrypt() - Exit");
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onFinalizeDecryptUnit
 */
status_t NvDrmPlugin::onFinalizeDecryptUnit(int uniqueId, 
					    DecryptHandle* decryptHandle, 
					    int decryptUnitId) 
{
  ALOGV("NvDrmPlugin::onFinalizeDecryptUnit() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onPread
 */
ssize_t NvDrmPlugin::onPread(int uniqueId, 
			     DecryptHandle* decryptHandle,
			     void* buffer, 
			     ssize_t numBytes, 
			     off64_t offset) 
{
  ALOGV("NvDrmPlugin::onPread() - Enter : %d", uniqueId);
  return 0;
}

