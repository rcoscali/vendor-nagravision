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

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define LOG_NDEBUG 0
#define LOG_TAG "DrmKernel"
#include <utils/Log.h>

#include "DrmKernel.h"
#include "nvDatabase.h"
#include "nvDatabaseSecureTable.h"

#define NULL	((void *)0)
#define NV_ASSERT(msg, expr) if ((expr)) { ALOGE("DrmKernel ASSERT Failure: " msg); exit(1); }

static char *C_databasePath = "/data/drm/nagravision/nv.db";
static NvDatabaseConnection mDatabaseConnection;
static int mDatabaseConnectionInitialized = 0;

/*
 * 
 */
#ifdef __cplusplus
extern "C"
#endif
static int
insertRecord(NvDatabaseConnection* pxDatabaseConnection,
	     SecureRecord* pxRecord) 
{
  int ret = 0;

  if (pxDatabaseConnection && pxRecord)
    {
      if (SQLITE_OK == openNvDatabase(pxDatabaseConnection))
	ret = (insertSecureRecord(pxDatabaseConnection->_pDatabase,
				  *pxRecord) == SQLITE_OK);
    }
  
  closeNvDatabase(pxDatabaseConnection);
  
  return ret;
}

/*
 * 
 */
#ifdef __cplusplus
extern "C"
#endif
static int 
getRecord(NvDatabaseConnection* pxDatabaseConnection,
	  const char* path, SecureRecord* pxRecord) 
{
  int ret = 0;
  
  if (pxDatabaseConnection && pxRecord) 
    {
      if (SQLITE_OK == openNvDatabase(pxDatabaseConnection)) 
	{
	  selectSecureRecord(pxDatabaseConnection->_pDatabase,
			     path, pxRecord); 
	  
	  ret = (pxRecord->_dataSize == 0);
	}
      
      closeNvDatabase(pxDatabaseConnection);
    }
  
  return ret;
}

/*
 *
 */
void
DrmKernel_init()
{
  if (!mDatabaseConnectionInitialized)
    {
      mDatabaseConnection._databaseName = C_databasePath ;
      mDatabaseConnection._pDatabase = (sqlite3*)NULL;
      mDatabaseConnectionInitialized = 1;
    }
}

/*
 *
 */
char *
DrmInfo_AttributeGet(const struct NV_DrmInfo_st *drmInfo, const char *key, unsigned int *dataSizePtr)
{
  char *data = (char *)NULL;
  unsigned int sz = 0;

  struct NV_DrmInfoAttribute_st *attr = drmInfo->pattributes;
  while (attr)
    {
      if (attr->name && key && 
	  !strncmp(attr->name, key, strlen(attr->name)))
	{
	  data = attr->value;
	  if (data)
	    sz = strlen(data);
	  break;
	}
      attr = attr->next;
    }

  if (dataSizePtr)
    *dataSizePtr = sz;
  
  return data;
}

/*
 *
 */
struct NV_DrmConstraints_st* 
DrmKernel_NvDrmPlugin_onGetConstraints(int uniqueId, const char *path, int action)
{
  struct NV_DrmConstraints_st *tmp = (struct NV_DrmConstraints_st *)malloc(sizeof(struct NV_DrmConstraints_st));
  if (tmp)
    {
      tmp->next = (struct NV_DrmConstraints_st *)NULL;
      tmp->key = strdup("license_available_time");
      tmp->value = strdup("dummy_available_time");
    }
  return tmp;
}

/*
 *
 */
struct NV_DrmInfoStatus_st *
DrmKernel_NvDrmPlugin_onProcessDrmInfo(int uniqueId, const struct NV_DrmInfo_st *drmInfo)
{
  struct NV_DrmInfoStatus_st *drmInfoStatus = (struct NV_DrmInfoStatus_st *)NULL;
  if (NULL != drmInfo)
    {
      switch (drmInfo->infoType)
	{
	case NV_DrmInfoRequest_TYPE_REGISTRATION_INFO: 
	  {
	    struct NV_DrmBuffer_st *emptyBuffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
	    NV_ASSERT("Buffer allocation error\n", emptyBuffer != NULL);
	    bzero(emptyBuffer, sizeof(struct NV_DrmBuffer_st));

	    drmInfoStatus = (struct NV_DrmInfoStatus_st *)malloc(sizeof(struct NV_DrmInfoStatus_st));
	    NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus != NULL);
	    bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

	    SecureRecord record;
	    record._key = "PERSO";
	    record._data = (unsigned char *)DrmInfo_AttributeGet(drmInfo, "PERSO", &record._dataSize);

	    drmInfoStatus->statusCode = 
	      insertRecord(&mDatabaseConnection, &record) ? 
	      NV_DrmInfoStatus_STATUS_OK : NV_DrmInfoStatus_STATUS_ERROR;
	    drmInfoStatus->infoType = NV_DrmInfoRequest_TYPE_REGISTRATION_INFO;
	    drmInfoStatus->drmBuffer = emptyBuffer;
	    if (drmInfo->mimeType != NULL)
	      drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
	  }
	  break;

	case NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO:
	  {
	    struct NV_DrmBuffer_st *emptyBuffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
	    NV_ASSERT("Buffer allocation error\n", emptyBuffer != NULL);
	    bzero(emptyBuffer, sizeof(struct NV_DrmBuffer_st));

	    drmInfoStatus = (struct NV_DrmInfoStatus_st *)malloc(sizeof(struct NV_DrmInfoStatus_st));
	    NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus != NULL);
	    bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

	    drmInfoStatus->statusCode = NV_DrmInfoStatus_STATUS_OK;
	    drmInfoStatus->infoType = NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO;
	    drmInfoStatus->drmBuffer = emptyBuffer;
	    if (drmInfo->mimeType != NULL)
	      drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
	  }
	  break;
	
	case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO:
	  {
	    struct NV_DrmBuffer_st *buffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
	    NV_ASSERT("Buffer allocation error\n", buffer != NULL);
	    bzero(buffer, sizeof(struct NV_DrmBuffer_st));
	    buffer->length = strlen("dummy_license_string");
	    buffer->data = (char *)malloc(buffer->length);
	    NV_ASSERT("Buffer data allocation error\n", buffer->data != NULL);
	    memcpy(buffer->data, "dummy_license_string", buffer->length);

	    drmInfoStatus = (struct NV_DrmInfoStatus_st *)malloc(sizeof(struct NV_DrmInfoStatus_st));
	    NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus != NULL);
	    bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

	    drmInfoStatus->statusCode = NV_DrmInfoStatus_STATUS_OK;
	    drmInfoStatus->infoType = NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO;
	    drmInfoStatus->drmBuffer = buffer;
	    if (drmInfo->mimeType != NULL)
	      drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
	  }
	  break;	  
	}
    }
  return drmInfoStatus;
}
