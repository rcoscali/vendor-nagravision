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

#define NULLSTR ((char *)0)
#define STR(x)  ((char *)x)
#define USTR(x) ((unsigned char *)x)

#define NV_ASSERT(msg, expr) if ((expr)) { ALOGE("DrmKernel ASSERT Failure: " msg); exit(1); }

static char *C_databasePath = "/data/drm/nagravision/nv.db";
static NvDatabaseConnection mDatabaseConnection;
static int mDatabaseConnectionInitialized = 0;

/*
 * @brief
 *   Utility func for inserting a SecureRecord in the DRM DB
 *
 * @param pxDatabaseConnection pointer to a struct describing the DB connection
 * @param pxRecord pointer to the record to insert
 *
 * @return 1 if record was successfully inserted. 0 otherwise
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
 * @brief
 *   Utility func for getting a record from DB
 *
 * @param pxDatabaseConnection pointer to a struct describing the DB connection
 * @param path 		allows to select the record to read from DB
 * @param pxRecord 	record read from DB and returned to rhe caller
 *
 * @return 1 if record was read successfully. 0 otherwise
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
 * @brief
 *   Initialize the DB connection. Called from the NvDrmPlugin constructor
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
 * @brief
 *   Helper func for accessing attributes from the NV_DrmInfo_st structure.
 *
 * @param drmInfo 	the NV_DrmInfo_st pointer from which to read attributes
 * @param key 		the key of the attribute to read
 * @param dataSizePtr	size of the attribute value if attribute was found for key
 *
 * @returns attribute value found for key 'key'. If noi attribute was found for key
 *          'key', returns NULL
 */
char *
DrmInfo_AttributeGet(const struct NV_DrmInfo_st *drmInfo, const char *key, unsigned int *dataSizePtr)
{
  char *data = NULLSTR;
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
void
DrmInfo_AttributePut(const struct NV_DrmInfo_st *drmInfo, const char *key, const char *val)
{
  unsigned int sz = 0;

  struct NV_DrmInfoAttribute_st *attr = drmInfo->pattributes;
  while (attr)
    {
      if (attr->name && key && 
	  !strncmp(attr->name, key, strlen(attr->name)))
	{
	  free ((void *)attr->value);
	  attr->value = STR(strdup(val));
	  NV_ASSERT("on DrmInfo:PutAttr(val)", attr->value);
	  return;
	}
      if (attr->next == (struct NV_DrmInfoAttribute_st *)NULL)
	break;
      attr = attr->next;
    }

  attr->next = (struct NV_DrmInfoAttribute_st *)malloc(sizeof(struct NV_DrmInfoAttribute_st));
  NV_ASSERT("on DrmInfo:PutAttr->next", attr->next);
  attr = attr->next;
  attr->name = (char *)strdup(key);
  NV_ASSERT("on DrmInfo:PutAttr->next->name", attr->name);
  attr->value = (char *)strdup(val);
  NV_ASSERT("on DrmInfo:PutAttr->next(val)", attr->value);
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
	    record._data = USTR(DrmInfo_AttributeGet(drmInfo, "PERSO", &record._dataSize));

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

/*
 *
 */
status_t
DrmKernel_NvDrmPlugin_onSaveRights(int uniqueId, 
		       const struct NV_DrmRights_st *drmRights,
		       const char *rightsPath,
		       const char *contentPath)
{
  status_t retVal = NV_DRM_ERROR_UNKNOWN;

  SecureRecord record;
  record._key = contentPath;
  record._data = USTR(drmRights->data->data);
  record._dataSize = (unsigned int)drmRights->data->length;

  if (insertRecord(&mDatabaseConnection, &record)) 
    {
      retVal = NV_NO_ERROR;
      ALOGV("DrmKernel_NvDrmPlugin_onSaveRights() - Rights saved");
    } 
  else 
    ALOGV("DrmKernel_NvDrmPlugin_onSaveRights() - Unabble to save rights");

  return retVal;
}

/*
 *
 */
struct NV_DrmInfo_st *
DrmKernel_NvDrmPlugin_onAcquireDrmInfo(int uniqueId, const struct NV_DrmInfoRequest_st *drmInfoRequest)
{
  struct NV_DrmInfo_st *drmInfo = (struct NV_DrmInfo_st *)NULL;

  if (drmInfoRequest != (struct NV_DrmInfoRequest_st *)NULL)
    {
      drmInfo = (struct NV_DrmInfo_st *)malloc(sizeof(struct NV_DrmInfo_st));
      NV_ASSERT("on DrmInfo", drmInfo);
      bzero((void*)drmInfo, sizeof(struct NV_DrmInfo_st));
      drmInfo->infoType = drmInfoRequest->infoType;
      drmInfo->mimeType = strdup(drmInfoRequest->mimeType);
      NV_ASSERT("on DrmInfo:mimeType", drmInfo->mimeType);

      switch (drmInfoRequest->infoType)
	{
	case NV_DrmInfoRequest_TYPE_REGISTRATION_INFO:
	  {
	    SecureRecord rec;
	    rec._key = "PERSO";
	    rec._data = (unsigned char *)NULL;
	    rec._dataSize = 0;
	    
	    if (getRecord(&mDatabaseConnection, "PERSO", &rec)) 
	      {
		DrmInfo_AttributePut(drmInfo, "PERSO", "yes");
		char *persodata = malloc(rec._dataSize+1);
		NV_ASSERT("on PersoData", persodata);
		strncpy(persodata, STR(rec._data), rec._dataSize);
		DrmInfo_AttributePut(drmInfo, "UNIQUE_ID", persodata);
		free((void*)persodata);
	      }
	  }
	  break;

	case NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO:
	  break;

	case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO:
	  break;

	case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO:
	  break;

	default:
	  NV_ASSERT("Invalid DrmInfoRequest:infoType", 1);
	}
    }
  else
    ALOGE("DrmKernel_onAcquireDrmInfo() - Invalid NULL drmInfoRequest param\n");

  return drmInfo;
}

/*
 *
 */
int 
DrmKernel_NvDrmPlugin_onCheckRightsStatus(int uniqueId, const char *path, int action)
{
  int rightsStatus = NV_RightsStatus_RIGHTS_INVALID;
  SecureRecord record;

  record._dataSize = 0;
  if (!getRecord(&mDatabaseConnection, path, &record))
    ALOGE("NvDrmPlugin::onCheckRightsStatus() - unable to get rights for %s", (const char* )path);
  else
    {
      if (strncmp((const char *)record._data, "0", record._dataSize))
	rightsStatus = NV_RightsStatus_RIGHTS_EXPIRED;
      else
	rightsStatus = NV_RightsStatus_RIGHTS_VALID;
    }

  return rightsStatus;
}

