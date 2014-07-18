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

  struct NV_DrmInfoAttribute {
    struct NV_DrmInfoAttribute *next;
    char *name;
    char *value;
  };

  struct NV_DrmInfo_st {
    int infoType;
    struct NV_DrmBuffer_st *drmBuffer;
    char *mimeType;
    struct NV_DrmInfoAttribute *attributes;
  };

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

  struct NV_DrmRights_st {
    struct NV_DrmBuffer_st *data;
    char *mimeType;
    char *accountId;
    char *subscriptionId;
    char *rightsFromFile;
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

  //  struct NV_DrmMetadata_st* DrmKernel_NvDrmPlugin_onGetMetadata(int uniqueId, const char *path);
  struct NV_DrmConstraints_st* DrmKernel_NvDrmPlugin_onGetConstraints(int uniqueId, const char *path, int action);
  struct NV_DrmInfoStatus_st* DrmKernel_NvDrmPlugin_onProcessDrmInfo(int uniqueId, const struct NV_DrmInfo_st *drmInfo);
  int DrmKernel_NvDrmPlugin_onSetOnInfoListener(int uniqueId, NV_OnInfoListener_OnInfoPtr lsnr);
  int DrmKernel_NvDrmPlugin_onInitialize(int uniqueId);
  int DrmKernel_NvDrmPlugin_onTerminate(int uniqueId);
  struct NV_DrmSupportInfo_st* DrmKernel_NvDrmPlugin_onGetSupportInfo(int uniqueId);
  int DrmKernel_NvDrmPlugin_onSaveRights(int uniqueId, struct NV_DrmRights_st *drmRights, const char *rightsPath, const char *contentPath);
  struct NV_DrmInfo_st* DrmKernel_NvDrmPlugin_onAcquireDrmInfo(int uniqueId, struct NV_DrmInfoRequest_st *drmInfoRequest);
  char DrmKernel_NvDrmPlugin_onCanHandle(int uniqueId, const char *path);
  char* DrmKernel_NvDrmPlugin_onGetOriginalMimeType(int uniqueId, const char *path, int fd);
  int DrmKernel_NvDrmPlugin_onGetDrmObjectType(int uniqueId, const char *path, const char *mimeType);
  int DrmKernel_NvDrmPlugin_onCheckRightsStatus(int uniqueId, const char *path, int action);
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
