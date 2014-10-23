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

#define NV_ASSERT(msg, expr) if ((!expr)) { ALOGE("DrmKernel ASSERT Failure: " msg); exit(1); }

static const char *mActionNames[NV_DRM_ACTION_NUMBER] = NV_DRM_ACTION_NAMES;

static char *C_databasePath = "/data/nv.db";
static NvDatabaseConnection mDatabaseConnection;
static int mDatabaseConnectionInitialized = 0;

/*
 * 
 */
#ifdef __cplusplus
extern "C"
#endif
static int insertRecord(NvDatabaseConnection* pxDatabaseConnection,
		SecureRecord* pxRecord) {
	ALOGV("insertRecord - Entry\n");
	int ret = 0;

	ALOGV("Record to insert: name = '%s' value = '%s' (sz=%d)\n",
			pxRecord->_key, pxRecord->_data, pxRecord->_dataSize);
	if (pxDatabaseConnection && pxRecord) {
		if (SQLITE_OK == openNvDatabase(pxDatabaseConnection))
			ret = (insertSecureRecord(pxDatabaseConnection->_pDatabase,
					*pxRecord) == SQLITE_OK);
	}

	closeNvDatabase(pxDatabaseConnection);

	ALOGV("insertRecord - Exit (%d)\n", ret);
	return ret;
}

/*
 * 
 */
#ifdef __cplusplus
extern "C"
#endif
static int getRecord(NvDatabaseConnection* pxDatabaseConnection,
		const char* path, SecureRecord* pxRecord) {
	ALOGV("getRecord - Entry\n");
	int ret = 0;


	if (pxDatabaseConnection && pxRecord) {
		if (SQLITE_OK == openNvDatabase(pxDatabaseConnection)) {
			selectSecureRecord(pxDatabaseConnection->_pDatabase, path,
					pxRecord);

			ret = pxRecord->_dataSize;
		}else{
			ALOGV("getRecord - cannot open this connection (%d)",pxDatabaseConnection );
		}

		closeNvDatabase(pxDatabaseConnection);
	}else{
		ALOGV("getRecord - bad db connection (%d) secure rec (%d)\n",pxDatabaseConnection, pxRecord );
	}

	ALOGV("getRecord - Exit (%d)\n", ret);
	return ret;
}

/*
 *
 */
void DrmKernel_init() {
	ALOGV("DrmKernel_init - Entry");

	if (!mDatabaseConnectionInitialized) {
		mDatabaseConnection._databaseName = C_databasePath;
		mDatabaseConnection._pDatabase = (sqlite3*) NULL;
		mDatabaseConnectionInitialized = 1;
	}

	ALOGV("DrmKernel_init - Exit");
}

/*
 *
 */
char *
DrmInfo_AttributeGet(const struct NV_DrmInfo_st *drmInfo, const char *key,
		unsigned int *dataSizePtr) {
	ALOGV("DrmInfo_AttributeGet - Entry");

	char *data = NULLSTR;
	unsigned int sz = 0;

	struct NV_DrmInfoAttribute_st *attr = drmInfo->pattributes;
	while (attr) {
		if (attr->name && key
				&& !strncmp(attr->name, key, strlen(attr->name))) {
			data = attr->value;
			if (data)
				sz = strlen(data);
			break;
		}
		attr = attr->next;
	}

	if (dataSizePtr)
		*dataSizePtr = sz;

	ALOGV("DrmInfo_AttributeGet - Exit (%10s)", data);
	return data;
}

/*
 *
 */
void DrmInfo_AttributePut(struct NV_DrmInfo_st *drmInfo, const char *key,
		const char *val) {
	ALOGV("DrmInfo_AttributePut - Entry (%s - %s)\n", key, val);

	unsigned int sz = 0;

	struct NV_DrmInfoAttribute_st *attr = drmInfo->pattributes;
	if (drmInfo->pattributes) {
		while (attr) {
			if (attr->name && key
					&& !strncmp(attr->name, key, strlen(attr->name))) {
				free((void *) attr->value);
				attr->value = STR(strdup(val));
				NV_ASSERT("on DrmInfo:PutAttr(val)", attr->value);
				return;
			}
			if (attr->next == (struct NV_DrmInfoAttribute_st *) NULL)
				break;
			else
				attr = attr->next;
		}

		attr->next = (struct NV_DrmInfoAttribute_st *) malloc(
				sizeof(struct NV_DrmInfoAttribute_st));
		NV_ASSERT("on DrmInfo:PutAttr->next", attr->next);
		attr = attr->next;
	} else {
		drmInfo->pattributes = (struct NV_DrmInfoAttribute_st *) malloc(
				sizeof(struct NV_DrmInfoAttribute_st));
		NV_ASSERT("on DrmInfo:PutAttr", drmInfo->pattributes);
		attr = drmInfo->pattributes;
	}

	attr->next = (struct NV_DrmInfoAttribute_st *) NULL;
	attr->name = (char *) strdup(key);
	NV_ASSERT("on DrmInfo:PutAttr->next->name", attr->name);
	attr->value = (char *) strdup(val);
	NV_ASSERT("on DrmInfo:PutAttr->next(val)", attr->value);

	ALOGV("DrmInfo_AttributePut - Exit\n");
}

/*
 *
 */
struct NV_DrmConstraints_st*
DrmKernel_NvDrmPlugin_onGetConstraints(int uniqueId, const char *path,
		int action) {
	ALOGV("DrmKernel_NvDrmPlugin_onGetConstraints - Entry\n");
	ALOGV("uniqueId = %d\n", uniqueId);
	ALOGV("path = '%s'\n", path);
	ALOGV("action = %d\n", action);

	struct NV_DrmConstraints_st *tmp = (struct NV_DrmConstraints_st *) malloc(
			sizeof(struct NV_DrmConstraints_st));
	if (tmp) {
		tmp->next = (struct NV_DrmConstraints_st *) NULL;
		tmp->key = strdup("license_available_time");
		tmp->value = strdup("dummy_available_time");
	}

	ALOGV("DrmKernel_NvDrmPlugin_onGetConstraints - Exit (%p)\n", tmp);
	return tmp;
}

/*
 *
 */
struct NV_DrmInfoStatus_st *
DrmKernel_NvDrmPlugin_onProcessDrmInfo(int uniqueId,
		const struct NV_DrmInfo_st *drmInfo) {
	ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - Entry\n");
	ALOGV("uniqueId = %d\n", uniqueId);

	struct NV_DrmInfoStatus_st *drmInfoStatus =
			(struct NV_DrmInfoStatus_st *) NULL;
	if (NULL != drmInfo) {
		switch (drmInfo->infoType) {

		case NV_DrmInfoRequest_TYPE_REGISTRATION_INFO: {
			ALOGV(
					"DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_REGISTRATION_INFO\n");

			struct NV_DrmBuffer_st *emptyBuffer =
					(struct NV_DrmBuffer_st *) malloc(
							sizeof(struct NV_DrmBuffer_st));
			NV_ASSERT("Buffer allocation error\n", emptyBuffer);
			bzero(emptyBuffer, sizeof(struct NV_DrmBuffer_st));

			drmInfoStatus = (struct NV_DrmInfoStatus_st *) malloc(
					sizeof(struct NV_DrmInfoStatus_st));
			NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
			bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

			SecureRecord record;
			record._key = "PERSO";
			record._data = USTR(
					DrmInfo_AttributeGet(drmInfo, "PERSO", &record._dataSize));

			drmInfoStatus->statusCode =
					insertRecord(&mDatabaseConnection, &record) ?
							NV_DrmInfoStatus_STATUS_OK :
							NV_DrmInfoStatus_STATUS_ERROR;
			drmInfoStatus->infoType = NV_DrmInfoRequest_TYPE_REGISTRATION_INFO;
			drmInfoStatus->drmBuffer = emptyBuffer;
			if (drmInfo->mimeType != NULL)
				drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
		}
			break;

		case NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO: {
			ALOGV(
					"DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_UNREGISTRATION_INFO\n");

			struct NV_DrmBuffer_st *emptyBuffer =
					(struct NV_DrmBuffer_st *) malloc(
							sizeof(struct NV_DrmBuffer_st));
			NV_ASSERT("Buffer allocation error\n", emptyBuffer);
			bzero(emptyBuffer, sizeof(struct NV_DrmBuffer_st));

			drmInfoStatus = (struct NV_DrmInfoStatus_st *) malloc(
					sizeof(struct NV_DrmInfoStatus_st));
			NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
			bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

			drmInfoStatus->statusCode = NV_DrmInfoStatus_STATUS_OK;
			drmInfoStatus->infoType =
					NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO;
			drmInfoStatus->drmBuffer = emptyBuffer;
			if (drmInfo->mimeType != NULL)
				drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
		}
			break;

		case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO: {
			ALOGV(
					"DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_RIGHTS_ACQUISITION_INFO\n");

			struct NV_DrmBuffer_st *buffer = (struct NV_DrmBuffer_st *) malloc(
					sizeof(struct NV_DrmBuffer_st));
			NV_ASSERT("Buffer allocation error\n", buffer);
			bzero(buffer, sizeof(struct NV_DrmBuffer_st));
			buffer->length = strlen("dummy_license_string");
			buffer->data = (char *) malloc(buffer->length);
			NV_ASSERT("Buffer data allocation error\n", buffer->data);
			memcpy(buffer->data, "dummy_license_string", buffer->length);

			drmInfoStatus = (struct NV_DrmInfoStatus_st *) malloc(
					sizeof(struct NV_DrmInfoStatus_st));
			NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
			bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

			drmInfoStatus->statusCode = NV_DrmInfoStatus_STATUS_OK;
			drmInfoStatus->infoType =
					NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO;
			drmInfoStatus->drmBuffer = buffer;
			if (drmInfo->mimeType != NULL)
				drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
		}
			break;

		case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO: {
			ALOGV(
					"DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO\n");

			struct NV_DrmBuffer_st *buffer = (struct NV_DrmBuffer_st *) malloc(
					sizeof(struct NV_DrmBuffer_st));
			NV_ASSERT("Buffer allocation error\n", buffer);
			bzero(buffer, sizeof(struct NV_DrmBuffer_st));
			buffer->length = strlen("dummy_license_string");
			buffer->data = (char *) malloc(buffer->length);
			NV_ASSERT("Buffer data allocation error\n", buffer->data);
			memcpy(buffer->data, "dummy_license_string", buffer->length);

			drmInfoStatus = (struct NV_DrmInfoStatus_st *) malloc(
					sizeof(struct NV_DrmInfoStatus_st));
			NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
			bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

			drmInfoStatus->statusCode = NV_DrmInfoStatus_STATUS_OK;
			drmInfoStatus->infoType =
					NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO;
			drmInfoStatus->drmBuffer = buffer;
			if (drmInfo->mimeType != NULL)
				drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
		}
			break;
		}
	}

	ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - Exit (%p)\n",
			drmInfoStatus);
	return drmInfoStatus;
}

/*
 *
 */
status_t DrmKernel_NvDrmPlugin_onSaveRights(int uniqueId,
		const struct NV_DrmRights_st *drmRights, const char *rightsPath,
		const char *contentPath) {
	ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - Entry\n");
	ALOGV("uniqueId = %d\n", uniqueId);
	ALOGV("rightsPath = '%s'\n", rightsPath);
	ALOGV("contentPath = '%s'\n", contentPath);

	status_t retVal = NV_DRM_ERROR_UNKNOWN;

	SecureRecord record;
	record._key = contentPath;
	record._data = USTR(drmRights->data->data);
	record._dataSize = (unsigned int) drmRights->data->length;

	if (insertRecord(&mDatabaseConnection, &record)) {
		retVal = NV_NO_ERROR;
		ALOGV("DrmKernel_NvDrmPlugin_onSaveRights() - Rights saved");
	} else
		ALOGE("DrmKernel_NvDrmPlugin_onSaveRights() - Unabble to save rights");

	ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - Exit (%d)\n", retVal);
	return retVal;
}

/*
 *
 */
struct NV_DrmInfo_st *
DrmKernel_NvDrmPlugin_onAcquireDrmInfo(int uniqueId,
		const struct NV_DrmInfoRequest_st *drmInfoRequest) {
	ALOGV("DrmKernel_NvDrmPlugin_onAcquireDrmInfo - Entry\n");

	struct NV_DrmInfo_st *drmInfo = (struct NV_DrmInfo_st *) NULL;

	if (drmInfoRequest != (struct NV_DrmInfoRequest_st *) NULL) {
		ALOGV("drmInfoRequest->infoType: %d\n", drmInfoRequest->infoType);
		ALOGV("drmInfoRequest->mimeType: '%s'\n", drmInfoRequest->mimeType);

		drmInfo = (struct NV_DrmInfo_st *) malloc(sizeof(struct NV_DrmInfo_st));
		NV_ASSERT("on DrmInfo", drmInfo);
		bzero((void*) drmInfo, sizeof(struct NV_DrmInfo_st));
		drmInfo->infoType = drmInfoRequest->infoType;
		if (drmInfoRequest->mimeType) {
			drmInfo->mimeType = strdup(drmInfoRequest->mimeType);
			NV_ASSERT("on DrmInfo:mimeType", drmInfo->mimeType);
		}

		struct NV_DrmBuffer_st *drmBuffer = (struct NV_DrmBuffer_st *) malloc(
				sizeof(struct NV_DrmBuffer_st));
		NV_ASSERT("Buffer allocation error\n", drmBuffer);
		bzero(drmBuffer, sizeof(struct NV_DrmBuffer_st));
		drmBuffer->data = (char *) strdup("Nagravision");
		drmBuffer->length = strlen("Nagravision");
		drmInfo->drmBuffer = drmBuffer;

		switch (drmInfoRequest->infoType) {
		case NV_DrmInfoRequest_TYPE_REGISTRATION_INFO: {
			SecureRecord rec;
			rec._key = "PERSO";
			rec._data = (unsigned char *) NULL;
			rec._dataSize = 0;

			ALOGV("Get perso record ...\n");
			if (getRecord(&mDatabaseConnection, "PERSO", &rec)) {
				DrmInfo_AttributePut(drmInfo, "PERSO", "yes");
				DrmInfo_AttributePut(drmInfo, "UNIQUE_ID", rec._data);
				free(rec._data);
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
			NV_ASSERT("Invalid DrmInfoRequest:infoType", 1)
			;
		}
	} else
		ALOGE(
				"DrmKernel_onAcquireDrmInfo() - Invalid NULL drmInfoRequest param\n");

	ALOGE("DrmKernel_onAcquireDrmInfo() - exit (%p)\n", drmInfo);
	return drmInfo;
}

/*
 *
 */
int DrmKernel_NvDrmPlugin_onCheckRightsStatus(int uniqueId, const char *path,
		int action) {
	ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Entry\n");
	int rightsStatus = NV_RightsStatus_RIGHTS_INVALID;
	SecureRecord record;
	record._key = path;
	record._data = (char *) NULL;
	record._dataSize = 0;

	if (!getRecord(&mDatabaseConnection, path, &record))
		ALOGE(
				"NvDrmPlugin::onCheckRightsStatus() - unable to get rights for %s",
				(const char*) path);
	else {
		if (!strncmp((const char *) record._data, "0", record._dataSize)) {
			rightsStatus = NV_RightsStatus_RIGHTS_EXPIRED;
			ALOGV(
					"DrmKernel_NvDrmPlugin_onCheckRightsStatus - Expired Rights\n");
		} else {
			rightsStatus = NV_RightsStatus_RIGHTS_VALID;
			ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Valid Rights\n");
			free(record._data);
		}
	}

	ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Exit\n");
	return rightsStatus;
}

