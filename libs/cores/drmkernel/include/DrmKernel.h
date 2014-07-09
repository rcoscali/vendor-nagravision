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

#ifdef __cplusplus
extern "C" {
#endif

  struct NV_DrmMetadata_st {
    struct NV_DrmMetadata_st *next;
    char *key;
    char *value;
  };

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
  }

  struct NV_DrmInfoRequest_st {
    int infoType;
    char *mimeType;
    struct NV_DrmRequestInfoMapNode_st  *requestInformationMap;
  };

  typedef void (NV_OnInfoListener_OnInfoPtr *)(struct NV_DrmInfoEvent_st *event);

  struct NV_DrmMetadata_st* DrmKernel_NvDrmPlugin_onGetMetadata(int uniqueId, const char *path);
  struct NV_DrmConstraints_st* DrmKernel_NvDrmPlugin_onGetConstraints(int uniqueId, const char *path, int action);
  struct NV_DrmInfoStatus_st* DrmKernel_NvDrmPlugin_onProcessDrmInfo(int uniqueId, const struct NV_DrmInfo_st *drmInfo);
  int DrmKernel_NvDrmPlugin_onSetOnInfoListener(int uniqueId, NV_OnInfoListener_OnInfoPtr *lsnr);
  int DrmKernel_NvDrmPlugin_onInitialize(int uniqueId);
  int DrmKernel_NvDrmPlugin_onTerminate(int uniqueId);
  struct NV_DrmSupportInfo_st* DrmKernel_NvDrmPlugin_onGetSupportInfo(int uniqueId);
  int DrmKernel_NvDrmPlugin_onSaveRights(int uniqueId, struct NV_DrmRights_st *drmRights, const char *rightsPath, const char *contentPath);
  struct NV_DrmInfo_st* DrmKernel_NvDrmPlugin_onAcquireDrmInfo(int uniqueId, struct NV_DrmInfoRequest_st *drmInfoRequest);

#ifdef __cplusplus
}
#endif

#endif /* __DRM_KERNEL_H__ */
