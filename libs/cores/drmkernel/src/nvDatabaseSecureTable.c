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

#include "nvDatabaseSecureTable.h"

int getFieldIndex(sqlite3_stmt* pxStatment, const char* xFieldName) {
  int ret = -1;
  int i = 0;
  int columnCount = sqlite3_column_count(pxStatment);
  ALOGV("nvDatabaseSecureTable::getFieldIndex() number of coulmns: %d ", columnCount);
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

int insertSecureRecord(sqlite3* pxDatabase, SecureRecord xRecord) {
  ALOGV("nvDatabaseSecureTable::insertSecureRecord() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = 0;
  for (;;) {
    // if the record exits delete it first
    (void) deleteSecureRecord(pxDatabase, xRecord._key);

    // Compile the INSERT statement into a virtual machine.
    rc = sqlite3_prepare(pxDatabase, gSecureTable._insertQuery, -1, &pStmt,
                         0);
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

    rc = sqlite3_bind_text(pStmt, 1, xRecord._key, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind key failed");
      break;
    }

    rc = sqlite3_bind_blob(pStmt, 2, xRecord._data, xRecord._dataSize,
                           SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::insertSecureRecord() - bind key data");
      break;
    }
    // Call sqlite3_step() to run the virtual machine. Since the SQL being
    // executed is not a SELECT statement, we assume no data will be returned.

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
      break;
    }
  }
  ALOGV("nvDatabaseSecureTable::insertSecureRecord() - Exit ");
  return rc;
}

int selectSecureRecord(sqlite3* pxDatabase, const char* xKey,
                       SecureRecord* pxRecord) {
  ALOGV("nvDatabaseSecureTable::selectSecureRecord() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = 0;
  char query[100];
  strcpy(query, gSecureTable._selectQuery);
  strcat(query, "'");
  strcat(query, xKey);
  strcat(query, "'");
  for (;;) {
    rc = sqlite3_prepare(pxDatabase, query, -1, &pStmt, 0);
    if (rc != SQLITE_OK) {
      ALOGV(
            "nvDatabaseSecureTable::selectSecureRecord() - prepare select query failed: %s",
            query);
      (void) sqlite3_finalize(pStmt);
      break;
    }

    rc = sqlite3_step(pStmt);
    if (rc != SQLITE_ROW) {
      ALOGV(
            "nvDatabaseSecureTable::selectSecureRecord() - select query execution failed: %s",
            query);
      (void) sqlite3_finalize(pStmt);
      break;
    }

    // The pointer returned by sqlite3_column_blob() points to memory
    // that is owned by the statement handle (pStmt). It is only good
    // until the next call to an sqlite3_XXX() function (e.g. the
    // sqlite3_finalize() below) that involves the statement handle.
    // So we need to make a copy of the blob into memory obtained from
    // malloc() to return to the caller.
    int index = getFieldIndex(pStmt, gSecureTable._dataColumn);
    if (index < 0) {
      ALOGV(
            "nvDatabaseSecureTable::selectSecureRecord() - unknown field index");
      (void) sqlite3_finalize(pStmt);
      break;
    }
    pxRecord->_dataSize = sqlite3_column_bytes(pStmt, index);
    pxRecord->_data = (unsigned char *) malloc(pxRecord->_dataSize);
    memcpy(pxRecord->_data, sqlite3_column_blob(pStmt, index),
           pxRecord->_dataSize);
    ALOGV(
          "nvDatabaseSecureTable::selectSecureRecord() - Record found: %s data size %d index %d",
          pxRecord->_key, pxRecord->_dataSize, index);

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
  strcat(query, "'");
  strcat(query, xKey);
  strcat(query, "'");

  for (;;) {
    rc = sqlite3_prepare(pxDatabase, query, -1, &pStmt, 0);
    if (rc != SQLITE_OK) {
      break;
    }

    // Call sqlite3_step() to run the virtual machine. Since the SQL being
    // executed is not a SELECT statement, we assume no data will be returned.

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
  ALOGV("nvDatabaseSecureTable::deleteSecureRecord() - Exit ");
  return rc;
}
