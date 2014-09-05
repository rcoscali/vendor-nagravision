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
#define LOG_TAG "drmNvToDroid"
#include <utils/Log.h>

#include "DrmKernel.h"
#include "drmNvToDroid.h"

/*
 * Return a struct NV_DrmConstraints_st initialized with a DrmConstraints
 * object.
 * 
 * @param in	Input structure to translate
 * @param inout Contains a pointer of pointer on a DrmConstraint object
 * 
 * @return 	pointer on DrmConstraints object. Is NULL is inout is NULL.
 */
DrmConstraints *
DrmConstraints_nv2droid(struct NV_DrmConstraints_st *in, DrmConstraints **inout)
{
  if (in == (struct NV_DrmConstraints_st *)NULL)
    {
      // Null entry -> return NULL
      ALOGE("DrmConstraints_nv2droid: Invalid null input param\n");
      return (DrmConstraints *)NULL;
    }

  if (inout != (DrmConstraints **)NULL)
    {
      if (*inout == (DrmConstraints *)NULL)
	*inout = new DrmConstraints();
    }
  else 
    {
      // Invalid call with new input object
      ALOGE("DrmConstraints_nv2droid: Invalid null input/output param\n");
      return NULL; 
    }

  struct NV_DrmConstraints_st *cur = in;

  while (cur)
    {
      (*inout)->put((const String8 *)new String8(in->key), in->value);
      cur = cur->next;
    }
  
  return *inout;    
}

/*
 * Return a struct NV_DrmBuffer_st initialized with a DrmBuffer
 * object.
 * 
 * @param in	Input structure to translate
 * @param inout Contains a pointer of pointer on a DrmConstraint object
 * 
 * @return 	pointer on DrmBuffer object. Is NULL is inout is NULL.
 */
DrmBuffer *
DrmBuffer_nv2droid(struct NV_DrmBuffer_st *in, DrmBuffer **inout)
{
  if (in == (struct NV_DrmBuffer_st *)NULL)
    {
      // Null entry -> return NULL
      ALOGE("DrmBuffer_nv2droid: Invalid null input param\n");
      return (DrmBuffer *)NULL;
    }

  if (inout != (DrmBuffer **)NULL)
    {
      if (*inout == (DrmBuffer *)NULL)
	{
	  *inout = new DrmBuffer();
	}
    }
  else 
    {
      // Invalid call with new input object
      ALOGE("DrmBuffer_nv2droid: Invalid null input/output param\n");
      return NULL; 
    }

  if (in->data != (char *)NULL && in->length > 0)
    {
      (*inout)->data = (char *)malloc(in->length);
      memcpy((*inout)->data, in->data, in->length);
    }

  return (*inout);
}

/*
 * Return a struct NV_DrmInfo_st initialized with a DrmInfo
 * object.
 * 
 * @param in	Input structure to translate
 * 
 * @return 	pointer on DrmInfo object. Is NULL is inout is NULL.
 */
DrmInfo *
DrmInfo_nv2droid(struct NV_DrmInfo_st *in)
{
  if (in == (struct NV_DrmInfo_st *)NULL)
    {
      // Null entry -> return NULL
      ALOGE("DrmInfo_nv2droid: Invalid null input param\n");
      return (DrmInfo *)NULL;
    }

  int info_type = 0;
  switch(in->infoType)
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

  DrmInfo *info = new DrmInfo(info_type, 
			      *DrmBuffer_nv2droid(in->drmBuffer, NULL), 
			      String8(in->mimeType));
  if (info != (DrmInfo *)NULL)
    {
      struct NV_DrmInfoAttribute_st *attr = in->pattributes;
      while (attr)
	{
	  info->put(String8(attr->name), String8(attr->value));
	  attr = attr->next;
	}
    }

  return info;
}

/*
 * Return a struct NV_DrmInfoStatus_st initialized with a DrmInfoStatus
 * object.
 * 
 * @param in	Input structure to translate
 * 
 * @return 	pointer on DrmInfoStatus object. Is NULL is inout is NULL.
 */
DrmInfoStatus *
DrmInfoStatus_nv2droid(struct NV_DrmInfoStatus_st *in)
{
  if (in == (struct NV_DrmInfoStatus_st *)NULL)
    {
      // Null entry -> return NULL
      ALOGE("DrmInfoStatus_nv2droid: Invalid null input param\n");
      return (DrmInfoStatus *)NULL;
    }

  int info_type = 0;
  switch(in->infoType)
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

  DrmInfoStatus *infoStatus = new DrmInfoStatus(in->statusCode, 
						info_type, 
						DrmBuffer_nv2droid(in->drmBuffer, NULL), 
						String8(in->mimeType));

  return infoStatus;
}

