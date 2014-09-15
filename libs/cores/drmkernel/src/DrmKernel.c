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

static NvDatabaseConnection mDatabaseConnection;
static int mDatabaseConnectionInitialized = 0;

static const char *mNagraRootCertPem = \
  "-----BEGIN CERTIFICATE-----"					   \
  "MIIELTCCAxWgAwIBAgIJAPEWvTHHh7OHMA0GCSqGSIb3DQEBCwUAMIGsMQswCQYD"	\
  "VQQGEwJDSDENMAsGA1UECAwEVmF1ZDERMA8GA1UEBwwIQ2hlc2VhdXgxFDASBgNV"	\
  "BAoMC05hZ3JhdmlzaW9uMRowGAYDVQQLDBFFbWJlZGRlZCBTZWN1cml0eTEjMCEG"	\
  "A1UEAwwaTmFncmEgRHJtIFJvb3QgQ2VydGlmaWNhdGUxJDAiBgkqhkiG9w0BCQEW"	\
  "FWRybWNhQG5hZ3JhdmlzaW9uLmNvbTAeFw0xNDA5MTIxNTA5NTlaFw0xNTA3MDkx"	\
  "NTA5NTlaMIGsMQswCQYDVQQGEwJDSDENMAsGA1UECAwEVmF1ZDERMA8GA1UEBwwI"	\
  "Q2hlc2VhdXgxFDASBgNVBAoMC05hZ3JhdmlzaW9uMRowGAYDVQQLDBFFbWJlZGRl"	\
  "ZCBTZWN1cml0eTEjMCEGA1UEAwwaTmFncmEgRHJtIFJvb3QgQ2VydGlmaWNhdGUx"	\
  "JDAiBgkqhkiG9w0BCQEWFWRybWNhQG5hZ3JhdmlzaW9uLmNvbTCCASIwDQYJKoZI"	\
  "hvcNAQEBBQADggEPADCCAQoCggEBANF/a7VtD/X2Yc/0g5dsuA61GGlUrZOgDXdj"	\
  "+Jl6dTm5ukLZfQKjKli+sMuGofVGJQeTQk/GeLXFWSBIOmBxQggkBygaGWor+bEy"	\
  "nT8RIb4OvowMIjzC+bCuF92OUMSfu2OeR6WkqjaQZ1JXQfuEiVjAKyQt1zDOfoEY"	\
  "cVFkcgIIq63Ui0nSKQJAGscXQSU/1mNeTF8TACyYRPS4T6R9tsbWBbKstaUTVuHI"	\
  "jsluYqSKhITNaalPnIl2Gk1RVN2m6jpi5HM9zmxkkfBNquRkQ/+adisen0d5wEgp"	\
  "Yy1RNQ5R7iJqvi60zP0Qnm8dxDxaeD4bwDUrG/voMv/pfnOkqscCAwEAAaNQME4w"	\
  "HQYDVR0OBBYEFLSRw3MgpsE6W5XavCl8Z+SEZYvVMB8GA1UdIwQYMBaAFLSRw3Mg"	\
  "psE6W5XavCl8Z+SEZYvVMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEB"	\
  "AJAhYAtI9dt3E3CXLEIrQzKsvdR4jEQEm4kwHWjTJnyi4Iwegq3lYxMDJStKJ+Uk"	\
  "PZQmBrIJSFagy/lxFoo+d4QMgfRDql3vXEpwQzPRUu5V31x6FtxAZK1XNOxk+DRr"	\
  "9fp5k9hxKBWeh3Gj0zhSES2I9ou5hMtr3zWU/ZalA4skFU9a8NeLeQbWPYwED3mk"	\
  "mMofbER4GXDUwtmmnVH7EzJAIOo4muCaylBuWbrSoEauTFL7ymmW7bnnD/0lGV3/"	\
  "p9CEbIwUpcvZTFskHO3DLy9XZnBUyi2GR+ufZToY7WlqQTakLrOYkwk/Jf9IrvHk"	\
  "nGrGEEfwdUZhrT/7J1utV3c="						\
  "-----END CERTIFICATE-----";

/* ================================================================================================
 * DB Helper funcs
 */

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
  ALOGV("insertRecord - Entry\n");
  int ret = 0;

  ALOGV("Record to insert: name = '%s' value = '%s' (sz=%d)\n", 
	pxRecord->_key, pxRecord->_data, pxRecord->_dataSize);
  if (pxDatabaseConnection && pxRecord)
    {
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
static int 
getRecord(NvDatabaseConnection* pxDatabaseConnection,
	  const char* path, SecureRecord* pxRecord) 
{
  ALOGV("getRecord - Entry\n");
  int ret = 0;
  
  if (pxDatabaseConnection && pxRecord) 
    {
      if (SQLITE_OK == openNvDatabase(pxDatabaseConnection)) 
	{
	  selectSecureRecord(pxDatabaseConnection->_pDatabase,
			     path, pxRecord); 
	  
	  ret = (pxRecord->_dataSize > 0);
	}
      
      closeNvDatabase(pxDatabaseConnection);
    }
  
  ALOGV("getRecord - Exit (%d)\n", ret);
  return ret;
}

/* ================================================================================================
 * Init
 */

/*
 *
 */
void
DrmKernel_init()
{
  ALOGV("DrmKernel_init - Entry");

  if (!mDatabaseConnectionInitialized)
    {
      mDatabaseConnection._databaseName = getNvDatabasePathname();
      mDatabaseConnection._pDatabase = (sqlite3*)NULL;
      mDatabaseConnectionInitialized = 1;
    }

  ALOGV("DrmKernel_init - Exit");
}

/* ================================================================================================
 * Linked list helper funcs (DrmInfo/DrmInfoRequest)
 */

/*
 *
 */
char *
DrmInfoRequest_InfoMapNodeGet(const struct NV_DrmInfoRequest_st *drmInfoRequest, const char *key, unsigned int *valueSizePtr)
{
  ALOGV("DrmInfoRequest_InfoMapNodeGet - Entry (key='%s')", key);

  char *value = NULLSTR;
  unsigned int sz = 0;

  struct NV_DrmRequestInfoMapNode_st *node = drmInfoRequest->requestInformationMap;
  while (node)
    {
      ALOGV("test %s", node->key);
      if (node->key && key && 
	  !strncmp(node->key, key, strlen(node->key)))
	{
	  ALOGV("found %s !", node->value);
	  value = node->value;
	  if (value)
	    sz = strlen(value);
	  break;
	}
      node = node->next;
    }

  if (valueSizePtr)
    *valueSizePtr = sz;
  
  ALOGV("DrmInfo_InfoMapNodeGet - Exit (%s)", value);
  return value;
}

/*
 *
 */
char *
DrmInfo_AttributeGet(const struct NV_DrmInfo_st *drmInfo, const char *key, unsigned int *dataSizePtr)
{
  ALOGV("DrmInfo_AttributeGet - Entry");

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
  
  ALOGV("DrmInfo_AttributeGet - Exit (%10s)", data);
  return data;
}

/*
 *
 */
void
DrmInfo_AttributePut(struct NV_DrmInfo_st *drmInfo, const char *key, const char *val)
{
  ALOGV("DrmInfo_AttributePut - Entry (%s - %s)\n", key, val);

  unsigned int sz = 0;

  struct NV_DrmInfoAttribute_st *attr = drmInfo->pattributes;
  if (drmInfo->pattributes)
    {
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
	  else
	    attr = attr->next;
	}

      attr->next = (struct NV_DrmInfoAttribute_st *)malloc(sizeof(struct NV_DrmInfoAttribute_st));
      NV_ASSERT("on DrmInfo:PutAttr->next", attr->next);
      attr = attr->next;
    }
  else
    {
      drmInfo->pattributes = (struct NV_DrmInfoAttribute_st *)malloc(sizeof(struct NV_DrmInfoAttribute_st));
      NV_ASSERT("on DrmInfo:PutAttr", drmInfo->pattributes);
      attr = drmInfo->pattributes;
    }

  attr->next = (struct NV_DrmInfoAttribute_st *)NULL;
  attr->name = (char *)strdup(key);
  NV_ASSERT("on DrmInfo:PutAttr->next->name", attr->name);
  attr->value = (char *)strdup(val);
  NV_ASSERT("on DrmInfo:PutAttr->next(val)", attr->value);

  ALOGV("DrmInfo_AttributePut - Exit\n");
}

/* ================================================================================================
 * Memory management funcs (free)
 */

void
DrmKernel_free_DrmBuffer(struct NV_DrmBuffer_st *drmBuffer)
{
  if (drmBuffer->data)
    free(drmBuffer->data);
}

void
DrmKernel_free_DrmRights(struct NV_DrmRights_st *drmRights)
{
  if (drmRights->data)
    DrmKernel_free_DrmBuffer(drmRights->data);
  if (drmRights->mimeType)
    free(drmRights->mimeType);
  if (drmRights->accountId)
    free(drmRights->accountId);
  if (drmRights->subscriptionId)
    free(drmRights->subscriptionId);
}

void
DrmKernel_free_DrmConstraints(struct NV_DrmConstraints_st *drmConstraints)
{
  while (drmConstraints)
    {
      struct NV_DrmConstraints_st *tmp = drmConstraints;
      drmConstraints = drmConstraints->next;
      if (tmp->key)
	free(tmp->key);
      if (tmp->value)
	free(tmp->value);
    }  
}

void
DrmKernel_free_DrmInfoStatus(struct NV_DrmInfoStatus_st *drmInfoStatus)
{
  if (drmInfoStatus->mimeType)
    free(drmInfoStatus->mimeType);
  if (drmInfoStatus->drmBuffer)
    DrmKernel_free_DrmBuffer(drmInfoStatus->drmBuffer);
}

void
DrmKernel_free_DrmInfoRequest(struct NV_DrmInfoRequest_st *drmInfoRequest)
{
  if (drmInfoRequest->mimeType)
    free(drmInfoRequest->mimeType);
  struct NV_DrmRequestInfoMapNode_st *rim = drmInfoRequest->requestInformationMap;
  if (rim)
    {
      struct NV_DrmRequestInfoMapNode_st *tmp = (struct NV_DrmRequestInfoMapNode_st *)NULL;
      do
	{
	  if (rim->key)
	    free(rim->key);
	  if (rim->value)
	    free(rim->value);
	  tmp = rim;
	  rim = rim->next;
	  free(tmp);
	}
      while (rim);
    }
}

void
DrmKernel_free_DrmInfo(struct NV_DrmInfo_st *drmInfo)
{
  if (drmInfo->drmBuffer)
    DrmKernel_free_DrmBuffer(drmInfo->drmBuffer);
  if (drmInfo->mimeType)
    free(drmInfo->mimeType);
  struct NV_DrmInfoAttribute_st *pattrs = drmInfo->pattributes;
  if (pattrs)
    {
      struct NV_DrmInfoAttribute_st *tmp = (struct NV_DrmInfoAttribute_st *)NULL;
      do
	{
	  if (pattrs->name)
	    free(pattrs->name);
	  if (pattrs->value)
	    free(pattrs->value);
	  tmp = pattrs;
	  pattrs = pattrs->next;
	  free(tmp);
	}
      while (pattrs);
    }
}

/* ================================================================================================
 * Drm Plugin impl
 */

/*
 *
 */
struct NV_DrmConstraints_st* 
DrmKernel_NvDrmPlugin_onGetConstraints(int uniqueId, const char *path, int action)
{
  ALOGV("DrmKernel_NvDrmPlugin_onGetConstraints - Entry\n");
  ALOGV("uniqueId = %d\n", uniqueId);
  ALOGV("path = '%s'\n", path);
  ALOGV("action = %d\n", action);

  struct NV_DrmConstraints_st *tmp = (struct NV_DrmConstraints_st *)malloc(sizeof(struct NV_DrmConstraints_st));
  bzero(tmp ,sizeof(struct NV_DrmConstraints_st));
  if (tmp)
    {
      tmp->next = (struct NV_DrmConstraints_st *)NULL;
      tmp->key = strdup("license_available_time");
      NV_ASSERT("on tmp->key alloc", tmp->key);
      tmp->value = strdup("dummy_available_time");
      NV_ASSERT("on tmp->value alloc", tmp->value);
    }

  ALOGV("DrmKernel_NvDrmPlugin_onGetConstraints - Exit (%p)\n", tmp);
  return tmp;
}

/*
 *
 */
struct NV_DrmInfoStatus_st *
DrmKernel_NvDrmPlugin_onProcessDrmInfo(int uniqueId, const struct NV_DrmInfo_st *drmInfo)
{
  ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - Entry\n");
  ALOGV("uniqueId = %d\n", uniqueId);

  struct NV_DrmInfoStatus_st *drmInfoStatus = (struct NV_DrmInfoStatus_st *)NULL;
  if (NULL != drmInfo)
    {
      bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));
      switch (drmInfo->infoType)
	{

	case NV_DrmInfoRequest_TYPE_REGISTRATION_INFO: 
	  {
	    ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_REGISTRATION_INFO\n");

	    struct NV_DrmBuffer_st *emptyBuffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
	    NV_ASSERT("Buffer allocation error\n", emptyBuffer);
	    bzero(emptyBuffer, sizeof(struct NV_DrmBuffer_st));

	    drmInfoStatus = (struct NV_DrmInfoStatus_st *)malloc(sizeof(struct NV_DrmInfoStatus_st));
	    NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
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
	    ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_UNREGISTRATION_INFO\n");

	    struct NV_DrmBuffer_st *emptyBuffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
	    NV_ASSERT("Buffer allocation error\n", emptyBuffer);
	    bzero(emptyBuffer, sizeof(struct NV_DrmBuffer_st));

	    drmInfoStatus = (struct NV_DrmInfoStatus_st *)malloc(sizeof(struct NV_DrmInfoStatus_st));
	    NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
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
	    ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_RIGHTS_ACQUISITION_INFO\n");

	    struct NV_DrmBuffer_st *buffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
	    NV_ASSERT("Buffer allocation error\n", buffer);
	    bzero(buffer, sizeof(struct NV_DrmBuffer_st));
	    buffer->length = strlen("dummy_license_string");
	    buffer->data = (char *)malloc(buffer->length);
	    NV_ASSERT("Buffer data allocation error\n", buffer->data);
	    memcpy(buffer->data, "dummy_license_string", buffer->length);

	    drmInfoStatus = (struct NV_DrmInfoStatus_st *)malloc(sizeof(struct NV_DrmInfoStatus_st));
	    NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
	    bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

	    drmInfoStatus->statusCode = NV_DrmInfoStatus_STATUS_OK;
	    drmInfoStatus->infoType = NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO;
	    drmInfoStatus->drmBuffer = buffer;
	    if (drmInfo->mimeType != NULL)
	      drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
	  }
	  break;	  

	case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO:
	  {
	    ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO\n");

	    struct NV_DrmBuffer_st *buffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
	    NV_ASSERT("Buffer allocation error\n", buffer);
	    bzero(buffer, sizeof(struct NV_DrmBuffer_st));
	    buffer->length = strlen("dummy_license_string");
	    buffer->data = (char *)malloc(buffer->length);
	    NV_ASSERT("Buffer data allocation error\n", buffer->data);
	    memcpy(buffer->data, "dummy_license_string", buffer->length);

	    drmInfoStatus = (struct NV_DrmInfoStatus_st *)malloc(sizeof(struct NV_DrmInfoStatus_st));
	    NV_ASSERT("DrmInfoStatus allocation error\n", drmInfoStatus);
	    bzero(drmInfoStatus, sizeof(struct NV_DrmInfoStatus_st));

	    drmInfoStatus->statusCode = NV_DrmInfoStatus_STATUS_OK;
	    drmInfoStatus->infoType = NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO;
	    drmInfoStatus->drmBuffer = buffer;
	    if (drmInfo->mimeType != NULL)
	      drmInfoStatus->mimeType = strdup(drmInfo->mimeType);
	  }
	  break;	  
	}
    }

  ALOGV("DrmKernel_NvDrmPlugin_onProcessDrmInfo - Exit (%p)\n", drmInfoStatus);
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
  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - Entry\n");
  ALOGV("uniqueId = %d\n", uniqueId);
  ALOGV("rightsPath = '%s'\n", rightsPath);
  ALOGV("contentPath = '%s'\n", contentPath);

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
    ALOGE("DrmKernel_NvDrmPlugin_onSaveRights() - Unable to save rights");

  ALOGV("DrmKernel_NvDrmPlugin_onSaveRights - Exit (%d)\n", retVal);
  return retVal;
}

static void
DrmKernel_do_signIn(const struct NV_DrmInfoRequest_st *drmInfoRequest, struct NV_DrmInfo_st *drmInfo)
{
  int success = 0;
  const char *username = DrmInfoRequest_InfoMapNodeGet(drmInfoRequest, "username", (unsigned int *)NULL);
  const char *password = DrmInfoRequest_InfoMapNodeGet(drmInfoRequest, "password", (unsigned int *)NULL);
  SecureRecord rec;
  rec._key = "PERSO";
  rec._data = (unsigned char *)NULL;
  rec._dataSize = 0;
  
  ALOGV("Get perso record ...\n");
  if (getRecord(&mDatabaseConnection, "PERSO", &rec)) 
    {
      char loginstr[50];
      sprintf(loginstr, "%s:%s:", username, password);
      success = !strncmp(loginstr, (const char *)rec._data, strlen(loginstr));
    }
  
  DrmInfo_AttributePut(drmInfo, "loginstatus", success ? "success" : "failure");
}

static void
DrmKernel_do_signUp(const struct NV_DrmInfoRequest_st *drmInfoRequest, struct NV_DrmInfo_st *drmInfo)
{
  int success = 0;
  char *persodata = NULL;
  size_t usernameLen, passwordLen, androididLen;
  const char *username = DrmInfoRequest_InfoMapNodeGet(drmInfoRequest, "username", (unsigned int *)&usernameLen);
  const char *password = DrmInfoRequest_InfoMapNodeGet(drmInfoRequest, "password", (unsigned int *)&passwordLen);
  const char *androidid = DrmInfoRequest_InfoMapNodeGet(drmInfoRequest, "androidid", (unsigned int *)&androididLen);
  persodata = malloc(usernameLen + passwordLen + androididLen + 3);
  NV_ASSERT("on persodata alloc", persodata);
  sprintf(persodata, "%s:%s:%s", username, password, androidid);
  
  SecureRecord rec;
  rec._key = "PERSO";
  rec._data = (unsigned char *)persodata;
  rec._dataSize = strlen(persodata);
  
  if (insertRecord(&mDatabaseConnection, &rec)) 
    success = 1;
  
  DrmInfo_AttributePut(drmInfo, "loginstatus", success ? "success" : "failure");
}

static void
DrmKernel_do_isDevicePersonalized(const struct NV_DrmInfoRequest_st *drmInfoRequest, struct NV_DrmInfo_st *drmInfo)
{
  size_t androidIdSz = 0;
  const char *androidId = DrmInfoRequest_InfoMapNodeGet(drmInfoRequest, "androidId", (unsigned int *)androidIdSz);

  SecureRecord rec;
  rec._key = "DEVCERT";
  rec._data = (unsigned char *)NULL;
  rec._dataSize = 0;
  if (getRecord(&mDatabaseConnection, rec._key, &rec))
    {
      DrmInfo_AttributePut(drmInfo, "deviceperso", "yes");
    }
  else
    {
      BIO *bio = (BIO *)NULL;
      BIO *bmd = (BIO *)NULL;
      BIO *inp = (BIO *)NULL;
      EVP_PKEY *pkey = (EVP_KEY *)NULL;
      X509 *cert = (X509 *)NULL;
      EVP_MD_CTX md_ctx;
      EVP_PKEY_CTX p_ctx;
      unsigned char md[SHA_DIGEST_LENGTH];
      char *devicePersoData = NULL;
      int r;

      bio = BIO_new(BIO_f_linebuffer());
      BIO_write(bio, (const void *)mNagraRootCertPem, strlen(mNagraRootCertPem));
      cert = PEM_read_bio_X509_REQ(in,NULL,NULL,NULL);
      BIO_ctrl(bio, BIO_CTRL_FLUSH, 0, (void *)NULL);
      pkey=X509_REQ_get_pubkey(cert);
      bmd = BIO_new(BIO_f_md());
      BIO_get_md_ctx(bmd, &md_ctx);
      EVP_MD_CTX_set_flags(md_ctx, EVP_MD_CTX_FLAG_NON_FIPS_ALLOW);
      if (EVP_DigestSignInit(md_ctx, &p_ctx, md, NULL, pkey))
	{
	  BIO_write(bio, androidId, strlen(androidId));
	  inp = BIO_push(bmd, bio);
	  if (BIO_read(inp, ))
	}
      
    }
}

/*
 *
 */
struct NV_DrmInfo_st *
DrmKernel_NvDrmPlugin_onAcquireDrmInfo(int uniqueId, const struct NV_DrmInfoRequest_st *drmInfoRequest)
{
  ALOGV("DrmKernel_NvDrmPlugin_onAcquireDrmInfo - Entry\n");

  struct NV_DrmInfo_st *drmInfo = (struct NV_DrmInfo_st *)NULL;

  if (drmInfoRequest != (struct NV_DrmInfoRequest_st *)NULL)
    {
      ALOGV("drmInfoRequest->infoType: %d\n", drmInfoRequest->infoType);
      ALOGV("drmInfoRequest->mimeType: '%s'\n", drmInfoRequest->mimeType);

      drmInfo = (struct NV_DrmInfo_st *)malloc(sizeof(struct NV_DrmInfo_st));
      NV_ASSERT("on DrmInfo", drmInfo);
      bzero((void*)drmInfo, sizeof(struct NV_DrmInfo_st));
      drmInfo->infoType = drmInfoRequest->infoType;
      if (drmInfoRequest->mimeType)
	{
	  drmInfo->mimeType = strdup(drmInfoRequest->mimeType);
	  NV_ASSERT("on DrmInfo:mimeType", drmInfo->mimeType);
	}

      struct NV_DrmBuffer_st *drmBuffer = (struct NV_DrmBuffer_st *)malloc(sizeof(struct NV_DrmBuffer_st));
      NV_ASSERT("Buffer allocation error\n", drmBuffer);
      bzero(drmBuffer, sizeof(struct NV_DrmBuffer_st));
      drmBuffer->data = (char *)strdup("Nagravision");
      drmBuffer->length = strlen("Nagravision");
      drmInfo->drmBuffer = drmBuffer;

      switch (drmInfoRequest->infoType)
	{
	case NV_DrmInfoRequest_TYPE_REGISTRATION_INFO:
	  {
            ALOGV("DrmKernel_NvDrmPlugin_onAcquireDrmInfo - TYPE_REGISTRATION_INFO\n");

	    const char *nvcmd = DrmInfoRequest_InfoMapNodeGet(drmInfoRequest, "NVCMD", (unsigned int *)NULL);

	    if (nvcmd && !strncmp(nvcmd, "SignIn", 6))
	      DrmKernel_do_signIn(drmInfoRequest, drmInfo);

	    else if (nvcmd && !strncmp(nvcmd, "SignUp", 6))
	      DrmKernel_do_signUp(drmInfoRequest, drmInfo);

	    else if (nvcmd && !strncmp(nvcmd, "DevicePerso?", 12))
	      DrmKernel_do_isDevicePersonalized(drmInfoRequest, drmInfo);

	    else
	      {
		SecureRecord rec;
		rec._key = "PERSO";
		rec._data = (unsigned char *)NULL;
		rec._dataSize = 0;
		
		ALOGV("Get perso record ...\n");
		if (getRecord(&mDatabaseConnection, "PERSO", &rec)) 
		  {
		    DrmInfo_AttributePut(drmInfo, "PERSO", "yes");
		    DrmInfo_AttributePut(drmInfo, "UNIQUE_ID", (const char *)rec._data);
		    free(rec._data);
		  }
	      }
	  }
	  break;

	case NV_DrmInfoRequest_TYPE_UNREGISTRATION_INFO:
          {
            ALOGV("DrmKernel_NvDrmPlugin_onAcquireDrmInfo - TYPE_UNREGISTRATION_INFO\n");
          }
	  break;

	case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_INFO:
          {
            ALOGV("DrmKernel_NvDrmPlugin_onAcquireDrmInfo - TYPE_RIGHTS_ACQUISITION_INFO\n");
          }
	  break;

	case NV_DrmInfoRequest_TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO:
          {
            ALOGV("DrmKernel_NvDrmPlugin_onAcquireDrmInfo - TYPE_RIGHTS_ACQUISITION_PROGRESS_INFO\n");
          }
	  break;

	default:
	  NV_ASSERT("Invalid DrmInfoRequest:infoType", 1);
	}
    }
  else
    ALOGE("DrmKernel_onAcquireDrmInfo() - Invalid NULL drmInfoRequest param\n");

  ALOGE("DrmKernel_onAcquireDrmInfo() - exit (%p)\n", drmInfo);
  return drmInfo;
}

/*
 *
 */
enum NV_RightsStatus_enum 
DrmKernel_NvDrmPlugin_onCheckRightsStatus(int uniqueId, const char *path, int action)
{
  ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Entry\n");

  int rightsStatus = NV_RightsStatus_RIGHTS_INVALID;
  SecureRecord record;
  record._key = path;
  record._data = (unsigned char *)NULL;
  record._dataSize = 0;

  if (!getRecord(&mDatabaseConnection, path, &record))
    {
      ALOGE("NvDrmPlugin::onCheckRightsStatus() - unable to get rights for %s", (const char* )path);
      rightsStatus = NV_RightsStatus_RIGHTS_NOT_ACQUIRED;
    }
  else
    {
      int rightsValue = atoi((const char *)record._data);
      if (rightsValue == 0)
	rightsStatus = NV_RightsStatus_RIGHTS_EXPIRED;
      else if (rightsValue > 0)
	rightsStatus = NV_RightsStatus_RIGHTS_VALID;
      free(record._data);
    }

  ALOGV("DrmKernel_NvDrmPlugin_onCheckRightsStatus - Exit (%d)\n", rightsStatus);
  return rightsStatus;
}

