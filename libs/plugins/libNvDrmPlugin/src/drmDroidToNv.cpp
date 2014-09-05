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

//#define LOG_NDEBUG 0
#define LOG_TAG "drmDroidToNv"
#include <utils/Log.h>

#include "DrmKernel.h"
#include "drmDroidToNv.h"

/*
 *
 */
struct NV_DrmInfo_st *
DrmInfo_droid2nv(DrmInfo *in)
{
  if (in == (struct DrmInfo *)NULL)
    {
      // Null entry -> return NULL
      ALOGE(TAG, "DrmInfo_droid2nv: Invalid null input param\n");
      return (struct NV_DrmInfo_st *)NULL;
    }

  // Alloc NV DrmInfo struct
  struct NV_DrmInfo_st *nvDrmInfo = (struct NV_DrmInfo_st *)malloc(sizeof(struct NV_DrmInfo_st));
  if (nvDrmInfo == (struct NV_DrmInfo_st *)NULL)
    {
      ALOGE(TAG, "DrmInfo_droid2nv: allocation error (1)\n");
      return (struct NV_DrmInfo_st *)NULL;
    }

  // Copy infoType & mimeType
  int info_type;
  switch (in->getInfoType())
    {
    case NV_DrmInfoRequest_TYPE_REGISTRATION_INFO:
      info_type = DrmInfoRequest::TYPE_REGISTRATION_INFO;
      break;
    case NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO:
      info_type = DrmInfoRequest::TYPE_UNREGISTRATION_INFO;
      break;
    case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO:
      info_type = DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_INFO;
      break;
    case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO:
      info_type = DrmInfoRequest::TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO;
      break;
    }
  nvDrmInfo->infoType = info_type;
  nvDrmInfo->mimeType = strdup(in->getMimeType().string());
  if (nvDrmInfo->mimeType == (char *)NULL)
    {
      free((void *)nvDrmInfo);
      ALOGE(TAG, "DrmInfo_droid2nv: allocation error (2)\n");
      return (struct NV_DrmInfo_st *)NULL;
    }

  // Copy DrmBuffer
  nvDrmInfo->drmBuffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
  if (nvDrmInfo->drmBuffer == NULL)
    {
      free((void *)nvDrmInfo->mimeType);
      free((void*)nvDrmInfo);
      ALOGE(TAG, "DrmInfo_droid2nv: allocation error (3)\n");
      return (struct NV_DrmInfo_st *)NULL;
    }
  nvDrmInfo->drmBuffer->length = in->getData().length;
  nvDrmInfo->drmBuffer->data = (char *)malloc(nvDrmInfo->drmBuffer->length);
  if (nvDrmInfo->drmBuffer->data == (char *)NULL)
    {
      free((void *)nvDrmInfo->drmBuffer);
      free((void *)nvDrmInfo->mimeType);
      free((void*)nvDrmInfo);
      ALOGE(TAG, "DrmInfo_droid2nv: allocation error (4)\n");
      return (struct NV_DrmInfo_st *)NULL;      
    }
  memcpy((void *)nvDrmInfo->drmBuffer->data, (void *)in->getData().data, nvDrmInfo->drmBuffer->length);

  // Copy attributes: iterate 
  struct NV_DrmInfoAttribute *attrs = (struct NV_DrmInfoAttribute *)NULL;
  for (DrmInfo::KeyIterator it = in.keyIterator();
       it.hasNext();)
    {
      // Allocate a new attr struct
      attrs = (struct NV_DrmInfoAttribute_st *)malloc(sizeof(struct NV_DrmInfoAttribute_st));
      if (attrs == (struct NV_DrmInfoAttribute_st *)NULL)
	goto DrmInfo_droid2nv_err;

      // Copy key & value
      String8 key = it.next();
      String8 val = in->get(key);
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
	  ALOGE(TAG, "DrmInfo_droid2nv: allocation error (5)\n");
	  return (struct NV_DrmInfo_st *)NULL;      
	}

      // This attr copy succeded, add it at list tail
      if (nvDrmInfo->pattributes == (struct NV_DrmInfoAttribute_st *)NULL)
	nvDrmInfo->pattributes = attrs;
      else
	{
	  struct NV_DrmInfoAttribute_st *tmp = nvDrmInfo->pattributes;
	  while (tmp->next != (struct NV_DrmInfoAttribute_st *)NULL) tmp = tmp->next;
	  tmp->next = attrs;
	}
    }

  return nvDrmInfo;
}
