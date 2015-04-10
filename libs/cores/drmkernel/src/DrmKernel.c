/*
 * Copyright (C) 2014-2015 NagraVision
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

#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#ifdef DRM_HOST_KERNEL_TEST
typedef __off64_t off64_t;
#endif

#include "DrmKernel.h"
#include "nvDatabase.h"
#include "nvDatabaseSecureTable.h"

#define NULL    ((void *)0)

#define NULLSTR ((char *)0)
#define STR(x)  ((char *)x)
#define USTR(x) ((unsigned char *)x)

#define NV_ASSERT(msg, expr) if ((!expr)) { ALOGE("DrmKernel ASSERT Failure: " msg); exit(1); }
#define NVMIN(a, b)  ((a) < (b) ? (a) : (b))

static const char *mActionNames[NV_DRM_ACTION_NUMBER] = NV_DRM_ACTION_NAMES;

static char *C_databasePath = C_NV_DATABASE_PATH;
static NvDatabaseConnection mDatabaseConnection;
static int mDatabaseConnectionInitialized = 0;

/* Rights confidentiality & authentication keys */
const uint8_t hmacKey[AES_BLOCK_SIZE] = {
  0xB7, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13, 
  0xA0, 0xB1, 0xC2, 0xD3, 0xE0, 0xF1, 0xE2, 0xF3
};
const uint8_t iv[AES_BLOCK_SIZE] = {
  0xA9, '2', '0', '1', '5', 'n', 'a', 'g', 'r', 'a', 'v', 'i', 's', 'i', 'o', 'n'
};


/*
 * 
 */
#ifdef DRM_HOST_KERNEL_TEST
#ifdef __cplusplus
extern "C"
#endif
static void printBuf(const char *msg, const uint8_t *buf, const size_t len)
{
  char outbuf[1024];
  unsigned int i = 0;

  bzero(outbuf, 1024);
  for (i = 0; i < NVMIN(len, 1024); i++)
    {
      uint8_t v = buf[i];
      sprintf(&outbuf[i*2], "%02x ", v);
    }
  ALOGV("%s = %s", msg, outbuf);
}
#else
#ifdef __cplusplus
extern "C"
#endif
static inline void printBuf(__attribute__((unused)) const char *msg, 
			    __attribute__((unused)) const uint8_t *buf, 
			    __attribute__((unused)) const size_t len)
{
  
}
#endif

/*
 * checkRecordCrypto
 *
 * Hash & Cipher a record for checking integrity & authenticity
 */
#ifdef __cplusplus
extern "C"
#endif
static int checkRecordCrypto(SecureRecord* pxRecord) {
  ALOGV("checkRecordCrypto - Entry");
  int ret = 0;

  if (pxRecord)
    {

      int outsz;
      unsigned char hash[SHA256_DIGEST_LENGTH+1];
      unsigned char tag[SHA256_DIGEST_LENGTH+1];
      uint8_t buf[256];
      unsigned int bufix = 0;
      
      SHA256_CTX sha256;
      EVP_CIPHER_CTX ectx;
      
      strncpy(STR(buf + bufix), pxRecord->_key, 255);
      bufix += NVMIN(strlen(pxRecord->_key)+1, 255);

      memcpy(STR(buf + bufix), pxRecord->_keyId, AES_BLOCK_SIZE);
      bufix += AES_BLOCK_SIZE;

      buf[bufix] = (pxRecord->_contentKeySize & 0xFF00)>>8;
      bufix++;

      buf[bufix] = (pxRecord->_contentKeySize & 0xFF);
      bufix++;

      memcpy(buf + bufix, pxRecord->_contentKey, pxRecord->_contentKeySize);
      bufix += pxRecord->_contentKeySize;

      SHA256_Init(&sha256);
      SHA256_Update(&sha256, buf, bufix);
      SHA256_Final(hash, &sha256);
      
      EVP_CIPHER_CTX_init(&ectx);
      EVP_EncryptInit_ex(&ectx, EVP_aes_128_cbc(), NULL, hmacKey, iv);
      EVP_EncryptUpdate(&ectx, tag, &outsz, hash, SHA256_DIGEST_LENGTH);

      /* Don't need to do the final step... 
	 no more blocks as sha 256 length is twice aes length 
      */
      
      if (memcmp (tag, pxRecord->_tag, SHA256_DIGEST_LENGTH) == 0)
	ret = 1;
      else
	ret = 0;
    }
  
  ALOGV("checkRecordCrypto - Exit (%d)\n", ret);
  return ret;
}

#ifdef __cplusplus
extern "C"
#endif
static int insertRecord(NvDatabaseConnection* pxDatabaseConnection,
                        SecureRecord* pxRecord) {
  ALOGV("insertRecord - Entry\n");
  int ret = 0;
  
  if (pxRecord) 
    {
      ALOGV("Record to insert: name = '%s'", pxRecord->_key);

      // Check if record is valid
      if (checkRecordCrypto(pxRecord)) 
	{
	  ALOGV("*** Record integrity & authenticity is valid !\n");
	  
	  if (pxDatabaseConnection) 
	    {
	      if (SQLITE_OK == openNvDatabase(pxDatabaseConnection))
		{
		  ret = (insertSecureRecord(pxDatabaseConnection->_pDatabase,
					    pxRecord) == SQLITE_OK);
		  ALOGV(   "insertRecord - saved record to db with:");
		  ALOGV(   "insertRecord -    _key            = %s", pxRecord->_key);
		  printBuf("insertRecord -    _keyId         ", pxRecord->_keyId, AES_BLOCK_SIZE);
		  printBuf("insertRecord -    _contentKey    ", pxRecord->_contentKey, pxRecord->_contentKeySize);
		  ALOGV(   "insertRecord -    _contentKeySize = %d", pxRecord->_contentKeySize);
		  printBuf("insertRecord -    _tag           ", pxRecord->_tag, pxRecord->_tagSize);
		  ALOGV(   "insertRecord -    _tagSize        = %d", pxRecord->_tagSize);
		  closeNvDatabase(pxDatabaseConnection);
		}
	      else
		{
		  ALOGE("insertRecord - Couldn't open database");
		}
	    }
	  
	}
      else
	{
	  ALOGV("*** Record integrity & authenticity is NOT valid !\n");
	}
    }
  
  ALOGV("insertRecord - Exit (%d)\n", ret);
  return ret;
}

static int insertPersoRecord(NvDatabaseConnection* pxDatabaseConnection,
			     SecureRecord* pxRecord) 
{
  ALOGV("insertPersoRecord - Entry\n");
  int ret = 0;
  
  if (pxRecord && !strncmp(pxRecord->_key, "PERSO", 5)) 
    {
      ALOGV("Record to insert: name = '%s'", pxRecord->_key);
      
      if (pxDatabaseConnection) 
	{
	  if (SQLITE_OK == openNvDatabase(pxDatabaseConnection))
	    {
	      ret = (insertSecureRecord(pxDatabaseConnection->_pDatabase,
					pxRecord) == SQLITE_OK);
	      ALOGV(   "insertRecord - saved record to db with:");
	      ALOGV(   "insertRecord -    _key            = %s", pxRecord->_key);
	      printBuf("insertRecord -    _keyId         ", pxRecord->_keyId, AES_BLOCK_SIZE);
	      printBuf("insertRecord -    _contentKey    ", pxRecord->_contentKey, pxRecord->_contentKeySize);
	      ALOGV(   "insertRecord -    _contentKeySize = %d", pxRecord->_contentKeySize);
	      printBuf("insertRecord -    _tag           ", pxRecord->_tag, pxRecord->_tagSize);
	      ALOGV(   "insertRecord -    _tagSize        = %d", pxRecord->_tagSize);
	      closeNvDatabase(pxDatabaseConnection);
	    }
	  else
	    {
	      ALOGE("insertPersoRecord - Couldn't open database");	      
	    }
	}
    }
  else
    {
      ALOGV("*** Record integrity & authenticity is NOT valid !\n");
    }
  
  ALOGV("insertPersoRecord - Exit (%d)\n", ret);
  return ret;
}

/*
 * 
 */
#ifdef __cplusplus
extern "C"
#endif
static int getRecord(NvDatabaseConnection* pxDatabaseConnection,
                     const char* pxKey, const uint8_t *pxKeyId, SecureRecord* pxRecord) {
  ALOGV("getRecord - Entry\n");
  int ret = 0;


  if (pxDatabaseConnection && pxRecord) 
    {
      if (SQLITE_OK == openNvDatabase(pxDatabaseConnection)) 
        {
          selectSecureRecord(pxDatabaseConnection->_pDatabase, pxKey, pxKeyId, 
                             pxRecord);

	  ALOGV(   "getRecord - got record from db with:");
	  ALOGV(   "getRecord -    pxKey = %s", pxKey);
	  printBuf("getRecord -    pxKeyId", pxKeyId, AES_BLOCK_SIZE);
	  ALOGV(   "getRecord - result set is:");
	  ALOGV(   "getRecord -    _key            = %s", pxRecord->_key);
	  printBuf("getRecord -    _keyId         ", pxRecord->_keyId, AES_BLOCK_SIZE);
	  printBuf("getRecord -    _contentKey    ", pxRecord->_contentKey, pxRecord->_contentKeySize);
	  ALOGV(   "getRecord -    _contentKeySize = %d", pxRecord->_contentKeySize);
	  printBuf("getRecord -    _tag           ", pxRecord->_tag, pxRecord->_tagSize);
	  ALOGV(   "getRecord -    _tagSize        = %d", pxRecord->_tagSize);
                
          ret = pxRecord->_tagSize + pxRecord->_contentKeySize;
        }
      else
        {
          ALOGV("getRecord - cannot open this connection (%p)", pxDatabaseConnection );
        }
            
      closeNvDatabase(pxDatabaseConnection);
    }
  else
    {
      ALOGV("getRecord - bad db connection (%p) secure rec (%p)\n",pxDatabaseConnection, pxRecord );
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
DrmInfo_AttributeGet(const struct NV_DrmInfo_st *drmInfo, 
                     const char *key,
                     unsigned int *dataSizePtr) 
{
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
void 
DrmInfo_AttributePut(struct NV_DrmInfo_st *drmInfo, 
                     const char *key,
                     const char *val) 
{
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
      record._contentKey = USTR("");
      record._contentKeySize = 0;
      record._tag = USTR(
                          DrmInfo_AttributeGet(drmInfo, "PERSO", &record._tagSize));

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
                                            const char *contentId) 
{
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - Entry\n");
  ALOGV("uniqueId = %d\n", uniqueId);
  ALOGV("rightsPath = '%s'\n", rightsPath);
  ALOGV("contentId = '%s'\n", contentId);

  status_t retVal = NV_DRM_ERROR_UNKNOWN;
  SecureRecord record;
  uint8_t *dataptr = (uint8_t *)drmRights->data->data;

  record._key = contentId;
  dataptr += strlen(contentId) +1;
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - index = %d", (dataptr - (uint8_t *)drmRights->data->data));

  record._keyId = dataptr;
  dataptr += AES_BLOCK_SIZE;
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - index = %d", (dataptr - (uint8_t *)drmRights->data->data));

  record._contentKeySize = *dataptr << 8;
  dataptr++;
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - index = %d", (dataptr - (uint8_t *)drmRights->data->data));

  record._contentKeySize += *dataptr;
  dataptr++;
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - index = %d", (dataptr - (uint8_t *)drmRights->data->data));

  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - key size = %d", record._contentKeySize);

  record._contentKey = USTR(dataptr);
  dataptr += record._contentKeySize;
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - index = %d", (dataptr - (uint8_t *)drmRights->data->data));

  record._tag = USTR(dataptr);
  record._tagSize = (unsigned int)drmRights->data->length - (dataptr - (uint8_t *)drmRights->data->data);
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - index = %d", (dataptr - (uint8_t *)drmRights->data->data));
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - tag size = %d", record._tagSize);

  ALOGV("record.key = %s\n", record._key);
  ALOGV("record.contentKeySize = %d\n", record._contentKeySize);
  ALOGV("record.tagSize = %d\n", record._tagSize);

      
  if (insertRecord(&mDatabaseConnection, &record)) 
    {
      retVal = NV_NO_ERROR;
      ALOGV("DrmKernel_NvDrmPlugin_onSaveRights() - Rights saved");
    } 
  else
    ALOGE("DrmKernel_NvDrmPlugin_onSaveRights() - Unable to save rights");
  
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - Exit (%d)\n", retVal);
  return retVal;
}

/*
 *
 */
struct NV_DrmInfo_st *
DrmKernel_NvDrmPlugin_onAcquireDrmInfo(__attribute__((unused)) int uniqueId,
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
      rec._keyId = (unsigned char *) NULL;
      rec._contentKey = (unsigned char *) NULL;
      rec._contentKeySize = 0;
      rec._tag = (unsigned char *) NULL;
      rec._tagSize = 0;

      ALOGV("Get perso record ...\n");
      if (getRecord(&mDatabaseConnection, "PERSO", NULL, &rec)) {
        DrmInfo_AttributePut(drmInfo, "PERSO", "yes");
        DrmInfo_AttributePut(drmInfo, "UNIQUE_ID", (const char *)rec._tag);
        if (rec._key) free((void *)rec._key);
        if (rec._keyId) free((void *)rec._keyId);
        if (rec._contentKey) free((void *)rec._contentKey);
        if (rec._tag) free((void *)rec._tag);
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
enum NV_RightsStatus_enum
DrmKernel_NvDrmPlugin_onCheckRightsStatus(__attribute__((unused)) int uniqueId, 
                                          const char *contentId,
                                          __attribute__((unused)) int action) 
{
  ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Entry\n");

  int rightsStatus = NV_RightsStatus_RIGHTS_INVALID;
  SecureRecord record;
  record._key = contentId;
  record._keyId = (unsigned char *) NULL;
  record._contentKey = (unsigned char *) NULL;
  record._contentKeySize = 0;
  record._tag = (unsigned char *) NULL;
  record._tagSize = 0;

  if (!getRecord(&mDatabaseConnection, contentId, NULL, &record))
    ALOGE(
          "NvDrmPlugin::onCheckRightsStatus() - unable to get rights for %s",
          (const char*) contentId);
  
  else 
    {
      if (!checkRecordCrypto(&record)) 
	{
	  ALOGV(
		"DrmKernel_NvDrmPlugin_onCheckRightsStatus - Invalid Rights\n");
	} 
      else 
	{
	  rightsStatus = NV_RightsStatus_RIGHTS_VALID;
	  ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Valid Right for content '%s'\n", contentId);
	  free((void *)record._key);
	  free((void *)record._keyId);
	  free((void *)record._contentKey);
	  free((void *)record._tag);
	}
    }

  ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Exit (%d)\n", rightsStatus);
  return rightsStatus;
}

#ifdef __cplusplus
extern "C"
#endif
__attribute__ ((visibility ("default"))) 
status_t DrmKernel_getRightKey(const uint8_t *xKeyId, uint8_t **ppxKey, size_t *pxKeylen)
{
  ALOGV("DrmKernel_getRightKey - Entry\n");

  status_t status = NV_DRM_ERROR_UNKNOWN;

  if (ppxKey && pxKeylen)
    {
      SecureRecord record;
      record._key = NULL;
      record._keyId = USTR(xKeyId);
      record._contentKey = (unsigned char *) NULL;
      record._contentKeySize = 0;
      record._tag = (unsigned char *) NULL;
      record._tagSize = 0;
      
      if (!getRecord(&mDatabaseConnection, NULL, xKeyId, &record))
	ALOGE(
	      "NvDrmPlugin::onGetRightKey() - unable to get right");
      
      else 
	{
	  if (!checkRecordCrypto(&record)) 
	    {
	      ALOGV(
		    "DrmKernel_getRightKey - Invalid Right\n");
	    } 
	  else 
	    {
	      ALOGV("DrmKernel_getRightKey - Valid Right\n");
	      
	      EVP_CIPHER_CTX ectx;

	      *ppxKey = (uint8_t *)malloc(record._contentKeySize);
	      if (!*ppxKey)
		{
		  ALOGE("DrmKernel_getRightKey - *ppxKey Allocation error !");
		  goto do_the_free;
		}
	      
	      EVP_CIPHER_CTX_init(&ectx);
	      EVP_DecryptInit_ex(&ectx, EVP_aes_128_cbc(), NULL, hmacKey, iv);
	      EVP_DecryptUpdate(&ectx, *ppxKey, (int *)pxKeylen, record._contentKey, record._contentKeySize);
	      EVP_DecryptFinal(&ectx, *ppxKey, (int *)pxKeylen);

	      status = NV_NO_ERROR;

	    do_the_free:
	      free((void *)record._key);
	      free((void *)record._keyId);
	      free((void *)record._contentKey);
	      free((void *)record._tag);
	    }
	}
    }
  else
    ALOGE("DrmKernel_getRightKey - Invalid parameters");

  ALOGV("DrmKernel_getRightKey - Exit (%d)\n", status);
  return status;  
}
