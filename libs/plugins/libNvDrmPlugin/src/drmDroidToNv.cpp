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

#include <string.h>

#define LOG_NDEBUG 0
#define LOG_TAG "drmDroidToNv"
#include <utils/Log.h>

#include "DrmKernel.h"
#include "drmDroidToNv.h"

using namespace android;

#define NV_ASSERT(msg, expr) if (!(expr)) { ALOGE("drmDroidToNv ASSERT Failure: " msg); exit(1); }

/*
 *
 */
struct NV_DrmInfo_st *
DrmInfo_droid2nv(const DrmInfo *in)
{
  ALOGV("DrmInfo_droid2nv - Enter");

  if (in == (struct DrmInfo *)NULL)
    {
      // Null entry -> return NULL
      ALOGE("DrmInfo_droid2nv: Invalid null input param\n");
      return (struct NV_DrmInfo_st *)NULL;
    }

  // Alloc NV DrmInfo struct
  struct NV_DrmInfo_st *nvDrmInfo = (struct NV_DrmInfo_st *)malloc(sizeof(struct NV_DrmInfo_st));
  if (nvDrmInfo == (struct NV_DrmInfo_st *)NULL)
    {
      ALOGE("DrmInfo_droid2nv: allocation error (1)\n");
      return (struct NV_DrmInfo_st *)NULL;
    }
  bzero(nvDrmInfo, sizeof(struct NV_DrmInfo_st));

  // Copy infoType & mimeType
  int info_type = 0;
  switch (in->getInfoType())
    {
    case DrmInfoRequest::TYPE_REGISTRATION_INFO:
      info_type = NV_DrmInfoRequest_TYPE_REGISTRATION_INFO;
      break;
    case DrmInfoRequest::TYPE_UNREGISTRATION_INFO:
      info_type = NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO;
      break;
    case DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_INFO:
      info_type = NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO;
      break;
    case DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO:
      info_type = NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO;
      break;
    }

  nvDrmInfo->infoType = info_type;
  nvDrmInfo->mimeType = strdup(in->getMimeType().string());
  if (nvDrmInfo->mimeType == (char *)NULL)
    {
      free((void *)nvDrmInfo);
      ALOGE("DrmInfo_droid2nv: allocation error (2)\n");
      return (struct NV_DrmInfo_st *)NULL;
    }

  // Copy DrmBuffer
  ALOGV("Copying data from DrmInfo: length = %d\n", in->getData().length);
  ALOGV("Copying data from DrmInfo: data = %s\n", in->getData().data);
  nvDrmInfo->drmBuffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
  if (nvDrmInfo->drmBuffer == NULL)
    {
      free((void *)nvDrmInfo->mimeType);
      free((void*)nvDrmInfo);
      ALOGE("DrmInfo_droid2nv: allocation error (3)\n");
      return (struct NV_DrmInfo_st *)NULL;
    }
  nvDrmInfo->drmBuffer->length = in->getData().length;
  nvDrmInfo->drmBuffer->data = (char *)malloc(nvDrmInfo->drmBuffer->length+1);
  if (nvDrmInfo->drmBuffer->data == (char *)NULL)
    {
      free((void *)nvDrmInfo->drmBuffer);
      free((void *)nvDrmInfo->mimeType);
      free((void*)nvDrmInfo);
      ALOGE("DrmInfo_droid2nv: allocation error (4)\n");
      return (struct NV_DrmInfo_st *)NULL;      
    }
  memcpy((void *)nvDrmInfo->drmBuffer->data, (void *)in->getData().data, nvDrmInfo->drmBuffer->length);
  nvDrmInfo->drmBuffer->data[nvDrmInfo->drmBuffer->length] = 0;

  // Copy attributes: iterate 
  ALOGV("Copying attrs from DrmInfo: %d attrs\n", in->getCount());
  struct NV_DrmInfoAttribute_st *attrs = (struct NV_DrmInfoAttribute_st *)NULL;
  for (DrmInfo::KeyIterator it = in->keyIterator();
       it.hasNext();)
    {
      // Declare these before goto
      String8 key, val;

      // Allocate a new attr struct
      attrs = (struct NV_DrmInfoAttribute_st *)malloc(sizeof(struct NV_DrmInfoAttribute_st));
      if (attrs == (struct NV_DrmInfoAttribute_st *)NULL)
	goto DrmInfo_droid2nv_err;
      bzero(attrs, sizeof(struct NV_DrmInfoAttribute_st));

      // Copy key & value
      key = it.next();
      val = in->get(key);
      ALOGV("Copying attrs from DrmInfo: key=%s val='%s'\n", key.string(), val.string());
      attrs->name = strdup(key.string());
      attrs->value = strdup(val.string());
      if (attrs->name == (char *)NULL || attrs->value == (char *)NULL)
	{
	  // If an error occured
	  if (attrs->name != (char *)NULL) free((void *)attrs->name);
	  if (attrs->value != (char *)NULL) free((void *)attrs->value);

	DrmInfo_droid2nv_err:
	  // Free list of attrs
	  while (nvDrmInfo->pattributes) 
	    {
	      struct NV_DrmInfoAttribute_st *tmp = nvDrmInfo->pattributes->next;
	      if (nvDrmInfo->pattributes->name != (char *)NULL)
		free((void *)nvDrmInfo->pattributes->name);
	      if (nvDrmInfo->pattributes->value != (char *)NULL)
		free((void *)nvDrmInfo->pattributes->value);
	      nvDrmInfo->pattributes = tmp;
	    }

	  free((void *)nvDrmInfo->drmBuffer);
	  free((void *)nvDrmInfo->mimeType);
	  free((void*)nvDrmInfo);
	  ALOGE("DrmInfo_droid2nv: allocation error (5)\n");
	  return (struct NV_DrmInfo_st *)NULL;      
	}

      // This attr copy succeded, add it at list tail
      if (nvDrmInfo->pattributes == (struct NV_DrmInfoAttribute_st *)NULL)
	{
	  ALOGV("Adding first attr\n");
	  nvDrmInfo->pattributes = attrs;
	}
      else
	{
	  ALOGV("Adding attr at tail\n");
	  struct NV_DrmInfoAttribute_st *tmp = nvDrmInfo->pattributes;
	  while (tmp->next != (struct NV_DrmInfoAttribute_st *)NULL) tmp = tmp->next;
	  tmp->next = attrs;
	}
    }

  return nvDrmInfo;
}

/*
 *
 */
struct NV_DrmRights_st *
DrmRights_droid2nv(const DrmRights *drmRights)
{
  struct NV_DrmRights_st *nvDrmRights = (struct NV_DrmRights_st *)malloc(sizeof(struct NV_DrmRights_st));
  NV_ASSERT("on nvDrmRights (1)", nvDrmRights);
  bzero(nvDrmRights, sizeof(struct NV_DrmRights_st));
  
  nvDrmRights->data = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
  NV_ASSERT("on nvDrmRights:data", nvDrmRights->data);
  nvDrmRights->data->length = drmRights->getData().length;
  nvDrmRights->data->data = (char *)malloc(drmRights->getData().length);
  NV_ASSERT("on nvDrmRights:data:data", nvDrmRights->data->data);
  memcpy((void *)nvDrmRights->data->data, 
	 (const void *)drmRights->getData().data,
	 drmRights->getData().length);

  if (!drmRights->getMimeType().isEmpty())
    {
      nvDrmRights->mimeType = (char *)strdup(drmRights->getMimeType().string());
      NV_ASSERT("on nvDrmRights:mimeType", nvDrmRights->mimeType);
    }
  else
    nvDrmRights->mimeType = NULL;
  
  if (!drmRights->getAccountId().isEmpty())
    {
      nvDrmRights->accountId = (char *)strdup(drmRights->getMimeType().string());
      NV_ASSERT("on nvDrmRights:accountId", nvDrmRights->accountId);
    }
  else
    nvDrmRights->accountId = NULL;
  
  if (!drmRights->getSubscriptionId().isEmpty())
    {
      nvDrmRights->subscriptionId = (char *)strdup(drmRights->getMimeType().string());
      NV_ASSERT("on nvDrmRights:subscriptionId", nvDrmRights->subscriptionId);
    }
  else
    nvDrmRights->subscriptionId = NULL;

  return nvDrmRights;
}

struct NV_DrmInfoRequest_st *
DrmInfoRequest_droid2nv(const DrmInfoRequest *drmInfoRequest)
{
  struct NV_DrmInfoRequest_st *nvDrmInfoRequest = (struct NV_DrmInfoRequest_st *)malloc(sizeof(struct NV_DrmInfoRequest_st));
  ALOGV("nvDrmInfoRequest = %p\n", nvDrmInfoRequest);
  NV_ASSERT("on DrmInfoRequest", nvDrmInfoRequest);

  // Copy infoType & mimeType
  int info_type = 0;
  switch (drmInfoRequest->getInfoType())
    {
    case DrmInfoRequest::TYPE_REGISTRATION_INFO:
      info_type = NV_DrmInfoRequest_TYPE_REGISTRATION_INFO;
      break;
    case DrmInfoRequest::TYPE_UNREGISTRATION_INFO:
      info_type = NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO;
      break;
    case DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_INFO:
      info_type = NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO;
      break;
    case DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO:
      info_type = NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO;
      break;
    }

  nvDrmInfoRequest->infoType = info_type;

  if (!drmInfoRequest->getMimeType().isEmpty())
    {
      nvDrmInfoRequest->mimeType = (char *)strdup(drmInfoRequest->getMimeType().string());
      NV_ASSERT("on DrmInfoRequest:mimeType", nvDrmInfoRequest->mimeType);
    }
  else
    {
      ALOGE("Invalid empty mimeType!\n");
      return (struct NV_DrmInfoRequest_st *)NULL;
    }

  // Copy attributes: iterate 
  struct NV_DrmRequestInfoMapNode_st *mapNode = (struct NV_DrmRequestInfoMapNode_st *)NULL;
  for (DrmInfoRequest::KeyIterator it = drmInfoRequest->keyIterator();
       it.hasNext();)
    {
      // Declare these before goto
      String8 key, val;

      // Allocate a new attr struct
      mapNode = (struct NV_DrmRequestInfoMapNode_st *)malloc(sizeof(struct NV_DrmRequestInfoMapNode_st));
      if (mapNode == (struct NV_DrmRequestInfoMapNode_st *)NULL)
	goto DrmInfoRequest_droid2nv_err;

      // Copy key & value
      key = it.next();
      val = drmInfoRequest->get(key);
      mapNode->key = strdup(key.string());
      mapNode->value = strdup(val.string());
      if (mapNode->key == (char *)NULL || mapNode->value == (char *)NULL)
	{
	  // If an error occured
	  if (mapNode->key != (char *)NULL) free((void *)mapNode->key);
	  if (mapNode->value != (char *)NULL) free((void *)mapNode->value);

	DrmInfoRequest_droid2nv_err:
	  // Free list of mapNode
	  while (nvDrmInfoRequest->requestInformationMap) 
	    {
	      struct NV_DrmRequestInfoMapNode_st *tmp = nvDrmInfoRequest->requestInformationMap->next;
	      if (nvDrmInfoRequest->requestInformationMap->key != (char *)NULL)
		free((void *)nvDrmInfoRequest->requestInformationMap->key);
	      if (nvDrmInfoRequest->requestInformationMap->value != (char *)NULL)
		free((void *)nvDrmInfoRequest->requestInformationMap->value);
	      nvDrmInfoRequest->requestInformationMap = tmp;
	    }

	  free((void *)nvDrmInfoRequest->mimeType);
	  free((void*)nvDrmInfoRequest);
	  ALOGE("DrmInfo_droid2nv: allocation error (5)\n");
	  return (struct NV_DrmInfoRequest_st *)NULL;      
	}

      // This attr copy succeded, add it at list tail
      if (nvDrmInfoRequest->requestInformationMap == (struct NV_DrmRequestInfoMapNode_st *)NULL)
	nvDrmInfoRequest->requestInformationMap = mapNode;
      else
	{
	  struct NV_DrmRequestInfoMapNode_st *tmp = nvDrmInfoRequest->requestInformationMap;
	  while (tmp->next != (struct NV_DrmRequestInfoMapNode_st *)NULL) tmp = tmp->next;
	  tmp->next = mapNode;
	}
    }

  
  return nvDrmInfoRequest;
}
