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

#include <malloc.h>

#define LOG_NDEBUG 0
#define LOG_TAG "nvDatabaseSecureTable"
#include <utils/Log.h>

#include <openssl/aes.h>

#include "nvDatabaseSecureTable.h"

/*
 * 
 */
#ifdef DRM_HOST_KERNEL_TEST
#define NVMIN(a, b)  ((a) < (b) ? (a) : (b))
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
static inline void printBuf(__attribute__((unused)) const char *msg, 
			    __attribute__((unused)) const uint8_t *buf, 
			    __attribute__((unused)) const size_t len)
{
  
}
#endif

int getFieldIndex(sqlite3_stmt* pxStatment, const char* xFieldName) {
  int ret = -1;
  int i = 0;
  int columnCount = sqlite3_column_count(pxStatment);
  ALOGV("nvDatabaseSecureTable::getFieldIndex() - number of coulmns: %d ", columnCount);
  for ( i = 0; i < columnCount; i++) {
    const char* name = sqlite3_column_name(pxStatment, i);
    ALOGV("nvDatabaseSecureTable::getFieldIndex() Field names: %s ", name);
    if (strncmp(name, xFieldName, strlen(xFieldName)) == 0) {
      ret = i;
      break;
    }
  }

  return ret;
}

int createSecureTable(sqlite3* pxDatabase) {
  ALOGV("nvDatabaseSecureTable::createSecureTable() - Enter ");
  int ret = sqlite3_exec(pxDatabase, gSecureTable._createQuery, 0, 0, 0);
  ALOGV("nvDatabaseSecureTable::createSecureTable() - Exit ");
  return ret;
}

int insertSecureRecord(sqlite3* pxDatabase, SecureRecord *pxRecord) {
  ALOGV("nvDatabaseSecureTable::insertSecureRecord() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = 0;
  for (;;) {
    // if the record exits delete it first
    (void) deleteSecureRecord(pxDatabase, pxRecord->_key);

    // Compile the INSERT statement into a virtual machine.
    rc = sqlite3_prepare_v2(pxDatabase, gSecureTable._insertQuery, -1, &pStmt, NULL);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - prepare insert query failed: ");
      break;
    }

    // Bind the key and value data for the new table entry to SQL variables
    // (the ? characters in the sql statement) in the compiled INSERT
    // statement.
    //
    // NOTE: variables are numbered from left to right from 1 upwards.
    // Passing 0 as the second parameter of an sqlite3_bind_XXX() function
    // is an error.

    // key
    rc = sqlite3_bind_text(pStmt, 1, pxRecord->_key, strlen(pxRecord->_key), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind key failed");
      break;
    }

    // kid
    rc = sqlite3_bind_blob(pStmt, 2, pxRecord->_keyId, AES_BLOCK_SIZE, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind content key failed");
      break;
    }

    // kclen
    rc = sqlite3_bind_int(pStmt, 3, pxRecord->_contentKeySize);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind content key size failed");
      break;
    }

    // kc
    ALOGV("nvDatabaseSecureTable::insertSecureRecord() - KCLEN = %d", pxRecord->_contentKeySize);
    printBuf("nvDatabaseSecureTable::insertSecureRecord() - KC ", pxRecord->_contentKey, pxRecord->_contentKeySize);
    rc = sqlite3_bind_blob(pStmt, 4, pxRecord->_contentKey, pxRecord->_contentKeySize, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind content key failed");
      break;
    }

    // taglen
    rc = sqlite3_bind_int(pStmt, 5, pxRecord->_tagSize);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind tag size failed");
      break;
    }

    // tag
    ALOGV("nvDatabaseSecureTable::insertSecureRecord() - TAGLEN = %d", pxRecord->_tagSize);
    printBuf("nvDatabaseSecureTable::insertSecureRecord() - TAG ", pxRecord->_tag, pxRecord->_tagSize);
    rc = sqlite3_bind_blob(pStmt, 6, pxRecord->_tag, pxRecord->_tagSize, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind tag failed");
      break;
    }

    // Call sqlite3_step() to run the virtual machine. Since the SQL being
    // executed is not a SELECT statement, we assume no data will be returned.

    rc = sqlite3_step(pStmt);
    if (rc == SQLITE_ROW) {
      (void) sqlite3_reset(pStmt);
      (void) sqlite3_finalize(pStmt);
      break;
    }
    else if (rc != SQLITE_DONE) {
      ALOGE(
            "nvDatabaseSecureTable::insertSecureRecord() - stmt exec failed: %s", sqlite3_errmsg(pxDatabase));
    }

    // Finalize the virtual machine. This releases all memory and other
    // resources allocated by the sqlite3_prepare() call above.

    (void) sqlite3_reset(pStmt);
    rc = sqlite3_finalize(pStmt);

    // If sqlite3_finalize() returned SQLITE_SCHEMA, then try to execute
    // the statement again.
    if (rc != SQLITE_SCHEMA) {
      break;
    }
  }
  ALOGV("nvDatabaseSecureTable::insertSecureRecord() - Exit ");
  return rc;
}

int selectSecureRecord(sqlite3* pxDatabase, const char* xKey, const uint8_t *xKeyId, 
                       SecureRecord* pxRecord) {
  ALOGV("nvDatabaseSecureTable::selectSecureRecord() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = 0;

  for (;;) {
    if (xKey)
      {
	rc = sqlite3_prepare_v2(pxDatabase, gSecureTable._selectQuery, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
	  ALOGE(
		"nvDatabaseSecureTable::selectSecureRecord() - prepare select query failed: %s",
		gSecureTable._selectQuery);
	  (void) sqlite3_finalize(pStmt);
	  break;
	}
	
	// key
	rc = sqlite3_bind_text(pStmt, 1, xKey, -1, SQLITE_TRANSIENT);
	if (rc != SQLITE_OK) {
	  ALOGE(
		"nvDatabaseSecureTable::selectSecureRecord() - bind key failed");
	  break;
	}
      }
    else if (xKeyId)
      {
	rc = sqlite3_prepare_v2(pxDatabase, gSecureTable._keyByKeyidQuery, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
	  ALOGE(
		"nvDatabaseSecureTable::selectSecureRecord() - prepare select query failed: %s",
		gSecureTable._keyByKeyidQuery);
	  (void) sqlite3_finalize(pStmt);
	  break;
	}
	
	// keyId
	rc = sqlite3_bind_blob(pStmt, 1, xKeyId, AES_BLOCK_SIZE, SQLITE_TRANSIENT);
	if (rc != SQLITE_OK) {
	  ALOGE(
		"nvDatabaseSecureTable::selectSecureRecord() - bind keyId failed");
	  break;
	}
      }
    else
      {
	ALOGE(
	      "nvDatabaseSecureTable::selectSecureRecord() - no select key specified");
	break;
      }

    rc = sqlite3_step(pStmt);
    if (rc != SQLITE_ROW) {
      ALOGE(
            "nvDatabaseSecureTable::selectSecureRecord() - select query execution failed");
      (void) sqlite3_finalize(pStmt);
      break;
    }

    // The pointer returned by sqlite3_column_blob() points to memory
    // that is owned by the statement handle (pStmt). It is only good
    // until the next call to an sqlite3_XXX() function (e.g. the
    // sqlite3_finalize() below) that involves the statement handle.
    // So we need to make a copy of the blob into memory obtained from
    // malloc() to return to the caller.
    int index = getFieldIndex(pStmt, "KEY");
    if (index < 0) {
      ALOGV(
            "nvDatabaseSecureTable::selectSecureRecord() - unknown field index for 'KEY' column in result set");
      (void) sqlite3_finalize(pStmt);
      break;
    }
    size_t keylen = sqlite3_column_bytes(pStmt, index);
    ALOGV("secureSelect - keylen = %d", keylen);
    pxRecord->_key = (char *) malloc(keylen+1);
    strcpy((char *)pxRecord->_key, (const char *)sqlite3_column_text(pStmt, index));
    ALOGV("secureSelect - key = %s", pxRecord->_key);

    index = getFieldIndex(pStmt, "KID");
    if (index < 0) {
      ALOGE(
            "nvDatabaseSecureTable::selectSecureRecord() - unknown field index for 'KID' column in result set");
      (void) sqlite3_finalize(pStmt);
      break;
    }
    size_t kidlen = sqlite3_column_bytes(pStmt, index);
    pxRecord->_keyId = (unsigned char *) malloc(kidlen);
    printBuf("nvDatabaseSecureTable::selectSecureRecord() - KID ", sqlite3_column_blob(pStmt, index), kidlen);
    memcpy(pxRecord->_keyId, sqlite3_column_blob(pStmt, index), kidlen);

    index = getFieldIndex(pStmt, "KCLEN");
    if (index < 0) {
      ALOGE(
            "nvDatabaseSecureTable::selectSecureRecord() - unknown field index for 'KCLEN' column in result set");
      (void) sqlite3_finalize(pStmt);
      break;
    }
    pxRecord->_contentKeySize = (unsigned int)sqlite3_column_int(pStmt, index);
    ALOGV("nvDatabaseSecureTable::selectSecureRecord() - KCLEN = %d", pxRecord->_contentKeySize);

    index = getFieldIndex(pStmt, "THEKC");
    if (index < 0) {
      ALOGE(
            "nvDatabaseSecureTable::selectSecureRecord() - unknown field index for 'KC' column in result set");
      (void) sqlite3_finalize(pStmt);
      break;
    }
    size_t kcsize = sqlite3_column_bytes(pStmt, index);
    ALOGV("nvDatabaseSecureTable::selectSecureRecord() - KC size = %d", kcsize);
    pxRecord->_contentKey = (unsigned char *) malloc(pxRecord->_contentKeySize);
    printBuf("nvDatabaseSecureTable::selectSecureRecord() - KC ", sqlite3_column_blob(pStmt, index), pxRecord->_contentKeySize);
    memcpy(pxRecord->_contentKey, sqlite3_column_blob(pStmt, index), pxRecord->_contentKeySize);

    index = getFieldIndex(pStmt, "TAGLEN");
    if (index < 0) {
      ALOGE(
            "nvDatabaseSecureTable::selectSecureRecord() - unknown field index for 'TAGLEN' column in result set");
      (void) sqlite3_finalize(pStmt);
      break;
    }
    pxRecord->_tagSize = (unsigned int)sqlite3_column_int(pStmt, index);
    ALOGV("nvDatabaseSecureTable::selectSecureRecord() - TAGLEN = %d", pxRecord->_tagSize);

    index = getFieldIndex(pStmt, "UNTAG");
    if (index < 0) {
      ALOGE(
            "nvDatabaseSecureTable::selectSecureRecord() - unknown field index for 'TAG' column in result set");
      (void) sqlite3_finalize(pStmt);
      break;
    }
    size_t tagsize = sqlite3_column_bytes(pStmt, index);
    ALOGV("nvDatabaseSecureTable::selectSecureRecord() - TAG size = %d", kcsize);
    pxRecord->_tag = (unsigned char *) malloc(pxRecord->_tagSize);
    printBuf("nvDatabaseSecureTable::selectSecureRecord() - TAG ", sqlite3_column_blob(pStmt, index), pxRecord->_tagSize);
    memcpy(pxRecord->_tag, sqlite3_column_blob(pStmt, index), pxRecord->_tagSize);

    ALOGV(
          "nvDatabaseSecureTable::selectSecureRecord() - Record found: %s keysize=%d tagsize=%d",
          pxRecord->_key, pxRecord->_contentKeySize, pxRecord->_tagSize);

    // Finalize the statement (this releases resources allocated by
    // sqlite3_prepare() ).
    rc = sqlite3_finalize(pStmt);

    // If sqlite3_finalize() returned SQLITE_SCHEMA, then try to execute
    // the statement all over again.
    if (rc != SQLITE_SCHEMA) {
      break;
    }
  }

  ALOGV("nvDatabaseSecureTable::selectSecureRecord() - Exit ");
  return rc;
}

int deleteSecureRecord(sqlite3* pxDatabase, const char* xKey) {
  ALOGV("nvDatabaseSecureTable::deleteSecureRecord() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = 0;

  char query[100];
  strcpy(query, gSecureTable._deleteQuery);

  for (;;) {
    rc = sqlite3_prepare_v2(pxDatabase, query, -1, &pStmt, 0);
    if (rc != SQLITE_OK) {
      break;
    }

    // key
    rc = sqlite3_bind_text(pStmt, 1, xKey, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
      ALOGE(
            "nvDatabaseSecureTable::selectSecureRecord() - bind key failed");
      break;
    }

    // Call sqlite3_step() to run the virtual machine. Since the SQL being
    // executed is not a SELECT statement, no data will be returned.

    rc = sqlite3_step(pStmt);
    if (rc == SQLITE_ROW) {
      (void) sqlite3_finalize(pStmt);
      break;
    }

    // Finalize the virtual machine. This releases all memory and other
    // resources allocated by the sqlite3_prepare() call above.

    rc = sqlite3_finalize(pStmt);

    // If sqlite3_finalize() returned SQLITE_SCHEMA, then try to execute
    // the statement again.
    if (rc != SQLITE_SCHEMA) {
      // return the number of rows affected
      rc = sqlite3_changes(pxDatabase);
      break;
    }
  }
  ALOGV("nvDatabaseSecureTable::deleteSecureRecord() - Exit (%d rows affected)", rc);
  return rc;
}
