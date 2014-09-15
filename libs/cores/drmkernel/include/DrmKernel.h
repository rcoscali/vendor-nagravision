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

#ifndef __DRM_KERNEL_H__
#define __DRM_KERNEL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* These must stay synced with defs in */
  /* system/core/include/utils/Error.h */ 
#define NV_OK			    (0)  
#define NV_NO_ERROR		    (0)  
#define NV_DRM_ERROR_BASE	(-2000)

  /* These MUST stay synced with defs in */
  /* frameworks/av/include/drm/drm_framework_common.h */
#define NV_DRM_ERROR_UNKNOWN				NV_DRM_ERROR_BASE
#define NV_DRM_ERROR_NO_LICENSE				NV_DRM_ERROR_BASE - 1
#define NV_DRM_ERROR_LICENSE_EXPIRED			NV_DRM_ERROR_BASE - 2
#define NV_DRM_ERROR_SESSION_NOT_OPENED			NV_DRM_ERROR_BASE - 3
#define NV_DRM_ERROR_DECRYPT_UNIT_NOT_INITIALIZED	NV_DRM_ERROR_BASE - 4
#define NV_DRM_ERROR_DECRYPT				NV_DRM_ERROR_BASE - 5
#define NV_DRM_ERROR_CANNOT_HANDLE			NV_DRM_ERROR_BASE - 6
#define NV_DRM_ERROR_TAMPER_DETECTED			NV_DRM_ERROR_BASE - 7
#define NV_DRM_ERROR_NO_PERMISSION			NV_DRM_ERROR_BASE - 8

  /* 
   * Errors
   */
  typedef int status_t;

  /*
   * Actions
   */
#define NV_DRM_ACTION_DEFAULT	0x00
#define NV_DRM_ACTION_PLAY	0x01
#define NV_DRM_ACTION_RINGTONE	0x02
#define NV_DRM_ACTION_TRANSFER	0x03
#define NV_DRM_ACTION_OUTPUT	0x04
#define NV_DRM_ACTION_PREVIEW	0x05
#define NV_DRM_ACTION_EXECUTE	0x06
#define NV_DRM_ACTION_DISPLAY	0x07
#define NV_DRM_ACTION_NUMBER	8

#define NV_DRM_ACTION_NAMES \
  {"action-DEFAULT",		\
   "action-PLAY",		\
   "action-RINGTONE",		\
   "action-TRANSFER",		\
   "action-OUTPUT",		\
   "action-PREVIEW",		\
   "action-EXECUTE",		\
   "action-DISPLAY"		\
  }


  /* struct NV_DrmMetadata_st { */
  /*   struct NV_DrmMetadata_st *next; */
  /*   char *key; */
  /*   char *value; */
  /* }; */

  struct NV_DrmConstraints_st {
    struct NV_DrmConstraints_st *next;
    char *key;
    char *value;
  };

  struct NV_DrmBuffer_st {
    char *data;
    int length;
  };

  struct NV_DrmInfoAttribute_st {
    struct NV_DrmInfoAttribute_st *next;
    char *name;
    char *value;
  };

#define NV_DrmInfoRequest_TYPE_REGISTRATION_INFO 		1
#define NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO 		2
#define NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO 		3
#define NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO 4

  struct NV_DrmInfo_st {
    int infoType;
    struct NV_DrmBuffer_st *drmBuffer;
    char *mimeType;
    struct NV_DrmInfoAttribute_st *pattributes;
  };

#define NV_DrmInfoStatus_STATUS_OK	1
#define NV_DrmInfoStatus_STATUS_ERROR	2

  struct NV_DrmInfoStatus_st {
    int statusCode;
    int infoType;
    struct NV_DrmBuffer_st *drmBuffer;
    char *mimeType;
  };

  struct NV_DrmInfoEvent_st {
    int uniqueId;
    int infoType;
    char *message;
  };

  struct NV_DrmSupportInfo_mimeTypeNode_st {
    char *mimeType;
    struct NV_DrmSupportInfo_mimeTypeNode_st *next;
  };

  struct NV_DrmSupportInfo_fileSuffixNode_st {
    char *fileSuffix;
    struct NV_DrmSupportInfo_mimeTypeNode_st *next;
  };

  struct NV_DrmSupportInfo_st {
    struct NV_DrmSupportInfo_mimeTypeNode_st *mimeTypes;
    struct NV_DrmSupportInfo_fileSuffixNode_st *fileSuffixes;
    char *description;
  };

  enum NV_RightsStatus_enum {
    NV_RightsStatus_RIGHTS_VALID = 0x00,
    NV_RightsStatus_RIGHTS_INVALID = 0x01,
    NV_RightsStatus_RIGHTS_EXPIRED = 0x02,
    NV_RightsStatus_RIGHTS_NOT_ACQUIRED = 0x03
  };

  struct NV_DrmRights_st {
    struct NV_DrmBuffer_st *data;
    char *mimeType;
    char *accountId;
    char *subscriptionId;
  };

  struct NV_DrmRequestInfoMapNode_st {
    struct NV_DrmRequestInfoMapNode_st *next;
    char *key;
    char *value;
  };

  struct NV_DrmInfoRequest_st {
    int infoType;
    char *mimeType;
    struct NV_DrmRequestInfoMapNode_st  *requestInformationMap;
  };

  struct NV_DecryptInfo_st {
    int decryptBufferLength;
  };

  struct NV_copyControlVectorNode_st {
    struct NV_copyControlVectorNode_st *next;
    int copyControlType; // At now only DHCP
    int n;
  };

  struct NV_ExtendedDataNode_st {
    struct NV_ExtendedDataNode_st *next;
    char *key;
    char *value;
  };

  struct NV_DecryptHandle_st {
    int decryptId;
    char *mimeType;
    int decryptApiType;
    int status;
    struct NV_DecryptInfo_st* decryptInfo;
    struct NV_copyControlVectorNode_st *copyControlVector;
    struct NV_ExtendedDataNode_st *extendedData;
  };

  typedef void (*NV_OnInfoListener_OnInfoPtr)(struct NV_DrmInfoEvent_st *event);

  void DrmKernel_init(void);

  void DrmKernel_free_DrmBuffer(struct NV_DrmBuffer_st *drmBuffer);
  void DrmKernel_free_DrmRights(struct NV_DrmRights_st *drmRights);
  void DrmKernel_free_DrmInfoStatus(struct NV_DrmInfoStatus_st *drmInfoStatus);
  void DrmKernel_free_DrmConstraints(struct NV_DrmConstraints_st *drmConstraints);
  void DrmKernel_free_DrmInfoRequest(struct NV_DrmInfoRequest_st *drmInfoRequest);
  void DrmKernel_free_DrmInfo(struct NV_DrmInfo_st *drmInfo);

  struct NV_DrmConstraints_st* DrmKernel_NvDrmPlugin_onGetConstraints(int uniqueId, const char *path, int action);
  struct NV_DrmInfoStatus_st* DrmKernel_NvDrmPlugin_onProcessDrmInfo(int uniqueId, const struct NV_DrmInfo_st *drmInfo);
  int DrmKernel_NvDrmPlugin_onSetOnInfoListener(int uniqueId, NV_OnInfoListener_OnInfoPtr lsnr);
  int DrmKernel_NvDrmPlugin_onInitialize(int uniqueId);
  int DrmKernel_NvDrmPlugin_onTerminate(int uniqueId);
  struct NV_DrmSupportInfo_st* DrmKernel_NvDrmPlugin_onGetSupportInfo(int uniqueId);
  status_t DrmKernel_NvDrmPlugin_onSaveRights(int uniqueId, const struct NV_DrmRights_st *drmRights, const char *rightsPath, const char *contentPath);
  struct NV_DrmInfo_st* DrmKernel_NvDrmPlugin_onAcquireDrmInfo(int uniqueId, const struct NV_DrmInfoRequest_st *drmInfoRequest);
  char DrmKernel_NvDrmPlugin_onCanHandle(int uniqueId, const char *path);
  char* DrmKernel_NvDrmPlugin_onGetOriginalMimeType(int uniqueId, const char *path, int fd);
  int DrmKernel_NvDrmPlugin_onGetDrmObjectType(int uniqueId, const char *path, const char *mimeType);
  enum NV_RightsStatus_enum DrmKernel_NvDrmPlugin_onCheckRightsStatus(int uniqueId, const char *path, int action);
  int DrmKernel_NvDrmPlugin_onConsumeRights(int uniqueId, struct NV_DecryptHandle_st *decryptHandle, int action, char reserve);
  /*                                                        check in order to replace void */
  int DrmKernel_NvDrmPlugin_onSetPlaybackStatus(int uniqueId,  struct NV_DecryptHandle_st *decryptHandle, int playbackStatus, int64_t position);
  char DrmKernel_NvDrmPlugin_onValidateAction(int uniqueId, const char *path, int action, int64_t position);
  int DrmKernel_NvDrmPlugin_onRemoveRights(int uniqueId, const char *path);
  int DrmKernel_NvDrmPlugin_onRemoveAllRights(int uniqueId);
  int DrmKernel_NvDrmPlugin_onOpenConvertSession(int uniqueId, int convertId);
  /* Check DrmConvertedStatus in order to replace void */
  void* DrmKernel_NvDrmPlugin_onConvertData(int uniqueId, int convertId, struct NV_DrmBuffer_st *inputData);
  void* DrmKernel_NvDrmPlugin_onCloseConvertSession(int uniqueId, int convertId);
  /*                                                        check in order to replace void */
  int DrmKernel_NvDrmPlugin_onOpenDecryptSessionFd(int uniqueId, struct NV_DecryptHandle_st *decryptHandle, int fd, off64_t offset, off64_t length);
  int DrmKernel_NvDrmPlugin_onOpenDecryptSessionUri(int uniqueId, struct NV_DecryptHandle_st *decryptHandle, const char *uri);
  int DrmKernel_NvDrmPlugin_onCloseDecryptSession(int uniqueId, struct NV_DecryptHandle_st *decryptHandle);
  int DrmKernel_NvDrmPlugin_onInitializeDecryptUnit(int uniqueId, struct NV_DecryptHandle_st *decryptHandle, int decryptUnitId, struct NV_DrmBuffer_st *headerInfo);
  int DrmKernel_NvDrmPlugin_onDecrypt(int uniqueId, struct NV_DecryptHandle_st *decryptHandle, int decryptUnitId, const struct NV_DrmBuffer_st *encBuffer, struct NV_DrmBuffer_st *decBuffer, struct NV_DrmBuffer_st *IV);
  int DrmKernel_NvDrmPlugin_onFinalizeaDecryptUnit(int uniqueId, struct NV_DecryptHandle_st *decryptHandle, int decryptUnitId);
  int DrmKernel_NvDrmPlugin_onPread(int uniqueId, struct NV_DecryptHandle_st *decryptHandle, void *buffer, ssize_t numBytes, off64_t offset);
  
  
  

#ifdef __cplusplus
}
#endif

#endif /* __DRM_KERNEL_H__ */
