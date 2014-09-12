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

#include <stdio.h>
#include <vector>
#include <string>
#include <map>

#define LOG_NDEBUG 0
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

#include <libxml/parser.h>

#include "nvDrmPlugin.h"
#include "parseMpdHelpers.h"
#include "drmNvToDroid.h"
#include "drmDroidToNv.h"
#include "DrmKernel.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "nvDatabaseSecureTable.h"
#include "sqlite3.h"

#define NV_ASSERT(msg, expr) if (!(expr)) { ALOGE("nvDrmPlugin: FATAL alloc error <" msg ">"); exit(255); }

#ifdef __cplusplus
}
#endif

using namespace android;

/* ==========================================================================
 * Static definitions for plugin
 * ==========================================================================*/

static NvDrmPlugin *sNvDrmPluginInstance = (NvDrmPlugin *)NULL;
static bool mapInitialized = false;

const char* const NvDrmPlugin::sNvMetadata[N_METADATA*2] = {
  "Copyright", "NagraVision ¢ 2014",
  "Author",    "Rémi Cohen-Scali"
};

String8 NvDrmPlugin::cencUuid = String8("urn:mpeg:dash:mp4protection:2011");

#define ADDMAP(x)				\
  static map<const char *, String8> x;		\
  String8					\
  NvDrmPlugin::x##Find(const char *key)		\
  {						\
    return x.find(key)->second;			\
  }
ADDMAP(drmSchemes);
ADDMAP(mimeTypes);
ADDMAP(fileExts);

/*
 * init ref maps
 */
#ifdef __cplusplus
extern "C" 
#endif
void 
initRefDataMaps()
{
#define MAPINSERT(x, y, z) x.insert(pair<const char *, String8>(y,String8(z)))

  // Supported DRM scheme UUIDs
  MAPINSERT(drmSchemes, "MSPlayReady", "9a04f079-9840-4286-ab92-e65be0885f95");
  MAPINSERT(  drmSchemes, "Dummy1", "00000000-0000-0000-0000-000000000001");
  MAPINSERT(  drmSchemes, "Dummy2", "00000000-0000-0000-0000-000000000002");
  MAPINSERT(  drmSchemes, "Nagravision", "deadbeef-dead-beef-2403-deadadd0beef");

  // Supported mimeTypes
  MAPINSERT(  mimeTypes, "nagra", "application/vnd.nagra.drm");
  MAPINSERT(  mimeTypes, "video1", "video/mp4");
  MAPINSERT(  mimeTypes, "audio1", "audio/mp4");
  MAPINSERT(  mimeTypes, "audio2", "audio/m4a");
  MAPINSERT(  mimeTypes, "video2", "video/m4v");
  MAPINSERT(  mimeTypes, "audio3", "audio/mpa");
  MAPINSERT(  mimeTypes, "video3", "video/mpv");
  MAPINSERT(  mimeTypes, "video4", "video/uvv");
  MAPINSERT(  mimeTypes, "audio4", "audio/uva");

  // Supported file extensions
  MAPINSERT(  fileExts, "mpd", ".mpd");
  MAPINSERT(  fileExts, "f1", ".nvv");
  MAPINSERT(  fileExts, "f2", ".mp4");
  MAPINSERT(  fileExts, "f3", ".m4a");
  MAPINSERT(  fileExts, "f4", ".m4v");
  MAPINSERT(  fileExts, "f5", ".mpa");
  MAPINSERT(  fileExts, "f6", ".mpv");
  MAPINSERT(  fileExts, "f7", ".uvv");
  MAPINSERT(  fileExts, "f8", ".uva");

#undef MAPINSERT

  mapInitialized = true;
}

/*
 * create
 *
 * This extern "C" is mandatory to be managed by TPlugInManager
 */
#ifdef __cplusplus
extern "C"
#endif
SYM_EXPORT IDrmEngine*
create() 
{
  ALOGV("IDrmEngine* create() - Enter");

  if (!mapInitialized)
    initRefDataMaps();

  if ((NvDrmPlugin *) NULL == sNvDrmPluginInstance)
    sNvDrmPluginInstance = new NvDrmPlugin();

  ALOGV("IDrmEngine* create() - Exit : instance=0x%p", sNvDrmPluginInstance);
  return (IDrmEngine*) sNvDrmPluginInstance;
}

/*
 * create
 *
 * This extern "C" is mandatory to be managed by TPlugInManager
 */
#ifdef __cplusplus
extern "C"
#endif
SYM_EXPORT IDrmEngine*
createDrmFactory() 
{
  ALOGV("IDrmEngine* createDrmfactory() - Enter");

  if (!mapInitialized)
    initRefDataMaps();

  if ((NvDrmPlugin *) NULL == sNvDrmPluginInstance)
    sNvDrmPluginInstance = new NvDrmPlugin();

  ALOGV("IDrmEngine* create() - Exit : instance=0x%p", sNvDrmPluginInstance);
  return (IDrmEngine*)sNvDrmPluginInstance;
}

/*
 * destroy
 *
 * This extern "C" is mandatory to be managed by TPlugInManager
 */
#ifdef __cplusplus
extern "C"
#endif
SYM_EXPORT void 
destroy(IDrmEngine* pPlugIn) 
{
  ALOGV("void destroy(IDrmEngine* pPlugIn) - Enter : 0x%p", pPlugIn);

  if (pPlugIn == (IDrmEngine *) sNvDrmPluginInstance) {
    delete sNvDrmPluginInstance;
    sNvDrmPluginInstance = (NvDrmPlugin *) NULL;
  } else
    delete pPlugIn;

  pPlugIn = (IDrmEngine *) NULL;

  ALOGV("void destroy(IDrmEngine* pPlugIn) - Exit");
}


/* ==========================================================================
 * Implementation of the DrmEngine class
 * ==========================================================================*/

/*
 * Public Constructor
 */
NvDrmPlugin::NvDrmPlugin() :
  DrmEngineBase() 
{
  ALOGV("NvDrmPlugin::NvDrmPlugin() - Enter");
  mNvDrmMetadata = new DrmMetadata();

  DrmKernel_init();

  for (int n = 0; n < N_METADATA / 2; n++) {
    String8 *key = new String8(NvDrmPlugin::sNvMetadata[2 * n]);
    String8 *value = new String8(NvDrmPlugin::sNvMetadata[2 * n + 1]);
    mNvDrmMetadata->put(key, (char const *) value);

    ALOGV("NvDrmPlugin::NvDrmPlugin(): added metadata key='%s' value='%s'",
	  key->string(), value->string());

    delete key;
    delete value;
  }
}

/* 
 * Virtual Public Destructor
 */
NvDrmPlugin::~NvDrmPlugin() 
{
  ALOGV("NvDrmPlugin::~NvDrmPlugin() - Enter");
}

/* ==========================================================================
 * Implementation of the DrmEngine
 * ==========================================================================*/

/*
 * NvDrmPlugin::onGetMetadata
 */
SYM_EXPORT DrmMetadata*
NvDrmPlugin::onGetMetadata(int uniqueId, const String8 *path) 
{
  ALOGV("NvDrmPlugin::onGetMetadata() - Enter : %d", uniqueId);
  return ((NULL != path) ? mNvDrmMetadata : (DrmMetadata*)NULL);
}

/*
 * NvDrmPlugin::onGetConstraints
 */
SYM_EXPORT DrmConstraints*
NvDrmPlugin::onGetConstraints(int uniqueId, const String8 *path, int action) 
{
  ALOGV("NvDrmPlugin::onGetConstraints() - Enter : %d", uniqueId);
  
  DrmConstraints *someDrmConstraints = (DrmConstraints *)NULL;
  
  struct NV_DrmConstraints_st *localConstraints =
    DrmKernel_NvDrmPlugin_onGetConstraints(uniqueId, path->string(), action);

  if (localConstraints != (struct NV_DrmConstraints_st *)NULL)
    {
      someDrmConstraints = new DrmConstraints();
      someDrmConstraints = DrmConstraints_nv2droid(localConstraints, &someDrmConstraints);
    }
  
  return someDrmConstraints;
}

/*
 * NvDrmPlugin::onProcessDrmInfo
 */
SYM_EXPORT DrmInfoStatus*
NvDrmPlugin::onProcessDrmInfo(int uniqueId, const DrmInfo *drmInfo) 
{
  ALOGV("NvDrmPlugin::onProcessDrmInfo() - Enter : %d", uniqueId);

  struct NV_DrmInfo_st *localDrmInfo = DrmInfo_droid2nv(drmInfo);
  NV_ASSERT("on DrmInfo", localDrmInfo);
  struct NV_DrmInfoStatus_st *localDrmInfoStatus = DrmKernel_NvDrmPlugin_onProcessDrmInfo(uniqueId, localDrmInfo);
  DrmInfoStatus *drmInfoStatus = DrmInfoStatus_nv2droid(localDrmInfoStatus);

  ALOGV("NvDrmPlugin::onProcessDrmInfo() - Exit");
  return drmInfoStatus;
}

/*
 * NvDrmPlugin::onSetOnInfoListener
 */
SYM_EXPORT status_t 
NvDrmPlugin::onSetOnInfoListener(int uniqueId,
				 const IDrmEngine::OnInfoListener *infoListener) 
{
  ALOGV("NvDrmPlugin::onSetOnInfoListener() - Enter : %d", uniqueId);

  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onInitialize
 */
SYM_EXPORT status_t 
NvDrmPlugin::onInitialize(int uniqueId)
{
  ALOGV("NvDrmPlugin::onInitialize() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onTerminate
 */
SYM_EXPORT status_t 
NvDrmPlugin::onTerminate(int uniqueId)
{
  ALOGV("NvDrmPlugin::onTerminate() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onGetSupportInfo
 */
SYM_EXPORT DrmSupportInfo* 
NvDrmPlugin::onGetSupportInfo(int uniqueId)
{
  ALOGV("NvDrmPlugin::onGetSupportInfo() - Enter : %d", uniqueId);

  DrmSupportInfo* drmSupportInfo = new DrmSupportInfo();

  // Add mimetype's
  for (map<const char *, String8>::iterator it = mimeTypes.begin();
       it != mimeTypes.end();
       ++it)
    drmSupportInfo->addMimeType(it->second);

  // Add File Suffixes
  for (map<const char *, String8>::iterator it = fileExts.begin();
       it != fileExts.end();
       ++it)
    drmSupportInfo->addFileSuffix(it->second);

  // Add plug-in description
  drmSupportInfo->setDescription(String8("NagraVision DRM plug-in"));

  ALOGV("NvDrmPlugin::onGetSupportInfo() : %d", uniqueId);
  return drmSupportInfo;
}

/*
 * NvDrmPlugin::onSaveRights
 */
SYM_EXPORT status_t 
NvDrmPlugin::onSaveRights(int uniqueId,
			  const DrmRights &drmRights, const String8 &rightsPath,
			  const String8 &contentPath) 
{
  ALOGV("NvDrmPlugin::onSaveRights() - Enter : %d", uniqueId);
  status_t retVal = DRM_ERROR_UNKNOWN;

  struct NV_DrmRights_st *localDrmRights = DrmRights_droid2nv(&drmRights);
  NV_ASSERT("on DrmRights", localDrmRights);
  retVal = DrmKernel_NvDrmPlugin_onSaveRights(uniqueId, 
				  localDrmRights, 
				  rightsPath.string(), 
				  contentPath.string());

  ALOGV("NvDrmPlugin::onSaveRights() - Exit");
  return retVal;
}

/*
 * NvDrmPlugin::onAcquireDrmInfo
 */
SYM_EXPORT DrmInfo*
NvDrmPlugin::onAcquireDrmInfo(int uniqueId,
			      const DrmInfoRequest *drmInfoRequest) 
{
  ALOGV("NvDrmPlugin::onAcquireDrmInfo() - Enter : %d\n", uniqueId);

  ALOGV("DrmInfoRequest.infoType = %d\n", drmInfoRequest->getInfoType());
  ALOGV("DrmInfoRequest.mimeType = '%s'\n", drmInfoRequest->getMimeType().string());

  DrmInfo* drmInfo = NULL;

  struct NV_DrmInfoRequest_st *localDrmInfoRequest = DrmInfoRequest_droid2nv(drmInfoRequest);
  NV_ASSERT("on DrmInfoRequest", localDrmInfoRequest);
  struct NV_DrmInfo_st *localDrmInfo = DrmKernel_NvDrmPlugin_onAcquireDrmInfo(uniqueId, localDrmInfoRequest);
  ALOGV("DrmInfo got from Drmkernel: %p", localDrmInfo);
  drmInfo = DrmInfo_nv2droid(localDrmInfo);
  ALOGV("DrmInfo we'll return: %p", drmInfo);

  ALOGV("NvDrmPlugin::onAcquireDrmInfo() - Exit (%p)", drmInfo);
  return drmInfo;
}

/*
 * NvDrmPlugin::parseMpd
 */
SYM_EXPORT bool
NvDrmPlugin::parseMpd(const String8 &path)
{
  bool cipherSchemeOk = false;
  bool drmSchemeOk = false;
  bool embedMediaOk = false;
  xmlDocPtr pDocument = NULL;
  xmlNodePtr pRootNode = NULL;

  if (access(path.string(), R_OK) == -1)
    {
      ALOGV("parseMPD: Media Descriptor not accessible !");
      return false;
    }
  pDocument = xmlParseFile(path.string());
  if (pDocument == NULL)
    return false;
  
  //ALOGV("MPD: pDocument = %p\n", pDocument);
  pRootNode = xmlDocGetRootElement(pDocument);
  //ALOGV("MPD: pRootNode = %p\n", pRootNode);
  if (pRootNode == NULL)
    return false;

  /*  print_element_names(pRootNode);*/
  cipherSchemeOk = findCencElement(pRootNode);
  ALOGV("findCencElement: >>> cipherSchemeOk %s\n", cipherSchemeOk ? "True" : "False");
  drmSchemeOk = findDrmScheme(pRootNode);
  ALOGV("findDrmScheme: >>> drmSchemeOk %s\n", drmSchemeOk ? "True" : "False");
  embedMediaOk = findSupportedMediaUri(pRootNode);

  ALOGV("Media from this descriptor are %sSUPPORTED\n", 
	(cipherSchemeOk && drmSchemeOk & embedMediaOk) ? "" : "NOT ");

  return (cipherSchemeOk && drmSchemeOk && embedMediaOk);
}

/*
 * NvDrmPlugin::onCanHandle
 */
SYM_EXPORT bool 
NvDrmPlugin::onCanHandle(int uniqueId, const String8 &path) 
{
  ALOGV("NvDrmPlugin::canHandle() - Enter : %d path='%s'", uniqueId, path.string());

  String8 extension = path.getPathExtension();
  extension.toLower();
  
  if (String8(".mpd") == extension) 
    {
      ALOGV("NvDrmPlugin::canHandle() - parsing MPD");
      return parseMpd(path);
    }

  for (map<const char *, String8>::iterator it = fileExts.begin();
       it != fileExts.end();
       ++it)
    {
      if (it->first != String8("mpd") &&
	  it->second == extension) 
	{
	  ALOGV("NvDrmPlugin::canHandle() - %s supported", it->second.string());
	  return true;
	}
    }

  ALOGV("NvDrmPlugin::canHandle() - NONE supported");
  return false;
}

/*
 * NvDrmPlugin::onGetOriginalMimeType
 */
SYM_EXPORT String8 
NvDrmPlugin::onGetOriginalMimeType(int uniqueId,
				   const String8 &path, int fd) 
{
  ALOGV("NvDrmPlugin::onGetOriginalMimeType() - Enter : %d path='%s'", uniqueId, path.string());
  String8 str("");
  for (map<const char *, String8>::iterator it = mimeTypes.begin();
       it != mimeTypes.end();
       ++it) 
    str.append(it->second + String8("; "));

  return str;
}

/*
 * NvDrmPlugin::onGetDrmObjectType
 */
SYM_EXPORT int 
NvDrmPlugin::onGetDrmObjectType(      int      uniqueId, 
				      const String8 &path, 
				      const String8 &mimeType) 
{
  ALOGV("NvDrmPlugin::onGetDrmObjectType() - Enter : %d", uniqueId);
  return DrmObjectType::UNKNOWN;
}

/*
 * NvDrmPlugin::onCheckRightsStatus
 */
SYM_EXPORT int 
NvDrmPlugin::onCheckRightsStatus(int uniqueId, const String8 &path, int action) 
{
  ALOGV("NvDrmPlugin::onCheckRightsStatus() - Enter : %d", uniqueId);
  int rightsStatus = RightsStatus::RIGHTS_NOT_ACQUIRED;

  ALOGV("NvDrmPlugin::onCheckRightsStatus() - Exit : %d", uniqueId);
  return rightsStatus;
}

/*
 * NvDrmPlugin::onConsumeRights
 */
SYM_EXPORT status_t
NvDrmPlugin::onConsumeRights(int            uniqueId, 
			     DecryptHandle *decryptHandle,
			     int            action, 
			     bool           reserve) 
{
  ALOGV("NvDrmPlugin::onConsumeRights() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onSetPlaybackStatus
 */
SYM_EXPORT status_t 
NvDrmPlugin::onSetPlaybackStatus(int            uniqueId, 
				 DecryptHandle *decryptHandle,
				 int            playbackStatus, 
				 int64_t        position) 
{
  ALOGV("NvDrmPlugin::onSetPlaybackStatus() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onValidateAction
 */
SYM_EXPORT bool 
NvDrmPlugin::onValidateAction(      int                uniqueId, 
				    const String8           &path,
			            int                action, 
				    const ActionDescription &description) 
{
  ALOGV("NvDrmPlugin::onValidateAction() - Enter : %d", uniqueId);
  return true;
}

/*
 * NvDrmPlugin::onRemoveRights
 */
SYM_EXPORT status_t 
NvDrmPlugin::onRemoveRights(      int      uniqueId, 
				  const String8 &path) 
{
  ALOGV("NvDrmPlugin::onRemoveRights() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onRemoveAllRights
 */
SYM_EXPORT status_t 
NvDrmPlugin::onRemoveAllRights(int uniqueId) 
{
  ALOGV("NvDrmPlugin::onRemoveAllRights() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onOpenConvertSession
 */
SYM_EXPORT status_t 
NvDrmPlugin::onOpenConvertSession(int uniqueId, 
				  int convertId) 
{
  ALOGV("NvDrmPlugin::onOpenConvertSession() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onConvertData
 */
SYM_EXPORT DrmConvertedStatus* 
NvDrmPlugin::onConvertData(      int        uniqueId, 
			         int        convertId, 
				 const DrmBuffer *inputData)
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
SYM_EXPORT DrmConvertedStatus* 
NvDrmPlugin::onCloseConvertSession(int uniqueId, 
				   int convertId) 
{
  ALOGV("NvDrmPlugin::onCloseConvertSession() - Enter : %d", uniqueId);
  return new DrmConvertedStatus(DrmConvertedStatus::STATUS_OK, NULL, 0 /*offset*/);
}

/*
 * NvDrmPlugin::onOpenDecryptSession
 */
SYM_EXPORT status_t 
NvDrmPlugin::onOpenDecryptSession(int            uniqueId, 
				  DecryptHandle *decryptHandle, 
				  int            fd, 
				  off64_t        offset, 
				  off64_t        length) 
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
SYM_EXPORT status_t 
NvDrmPlugin::onOpenDecryptSession(      int            uniqueId, 
				        DecryptHandle *decryptHandle, 
					const char          *uri) 
{
  return DRM_ERROR_CANNOT_HANDLE;
}

/*
 * NvDrmPlugin::onCloseDecryptSession
 */
SYM_EXPORT status_t 
NvDrmPlugin::onCloseDecryptSession(int            uniqueId, 
				   DecryptHandle *decryptHandle) 
{
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
SYM_EXPORT status_t 
NvDrmPlugin::onInitializeDecryptUnit(      int            uniqueId, 
				           DecryptHandle *decryptHandle,
				           int            decryptUnitId, 
					   const DrmBuffer     *headerInfo) 
{
  ALOGV("NvDrmPlugin::onInitializeDecryptUnit() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onDecrypt
 */
SYM_EXPORT status_t 
NvDrmPlugin::onDecrypt(      int             uniqueId, 
		             DecryptHandle  *decryptHandle,
		             int             decryptUnitId, 
			     const DrmBuffer      *encBuffer, 
		             DrmBuffer     **decBuffer, 
		             DrmBuffer      *IV) 
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
	  ALOGV("decBuffer size (%d) too small to hold %d bytes",
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
SYM_EXPORT status_t 
NvDrmPlugin::onFinalizeDecryptUnit(int            uniqueId, 
				   DecryptHandle *decryptHandle, 
				   int            decryptUnitId) 
{
  ALOGV("NvDrmPlugin::onFinalizeDecryptUnit() - Enter : %d", uniqueId);
  return DRM_NO_ERROR;
}

/*
 * NvDrmPlugin::onPread
 */
SYM_EXPORT ssize_t 
NvDrmPlugin::onPread(int            uniqueId, 
		     DecryptHandle *decryptHandle,
		     void          *buffer, 
		     ssize_t        numBytes, 
		     off64_t        offset) 
{
  ALOGV("NvDrmPlugin::onPread() - Enter : %d", uniqueId);
  return 0;
}

// Local Variables:
// mode:C++
// End:
