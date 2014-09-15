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


int 
createSecureTable(sqlite3* pxDatabase)
{
  ALOGV("nvDatabaseSecureTable::createSecureTable() - Enter ");

  int ret = sqlite3_exec(pxDatabase, gSecureTable._createQuery, 0, 0, 0);

  ALOGV("nvDatabaseSecureTable::createSecureTable() - Exit ");
  return ret;
}

int
insertSecureRecord(sqlite3* pxDatabase, SecureRecord xRecord)
{
  ALOGV("nvDatabaseSecureTable::insertSecureRecord() - Enter ");

  sqlite3_stmt *pStmt = 0;
  int rc = 0;

  do
    {
      (void)deleteSecureRecord(pxDatabase, xRecord._key);
      if((rc = sqlite3_prepare_v2(pxDatabase, 
				  gSecureTable._insertQuery, 
				  -1, &pStmt, 0)) !=SQLITE_OK ||
	 (rc = sqlite3_bind_text(pStmt, 1, 
				 xRecord._key, -1, 
				 SQLITE_TRANSIENT)) !=SQLITE_OK ||
	 (rc = sqlite3_bind_blob(pStmt, 2, 
				 xRecord._data, 
				 xRecord._dataSize, 
				 SQLITE_TRANSIENT)) != SQLITE_OK)
	break;
      if((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
	{
	  (void)sqlite3_finalize(pStmt);
	  break;
	}
      rc = sqlite3_finalize(pStmt);
      if(rc != SQLITE_SCHEMA)
	break;
    } 
  while(0);

  ALOGV("nvDatabaseSecureTable::insertSecureRecord() - Exit ");
  return rc;
}

int selectSecureRecord(sqlite3* pxDatabase, const char* xKey, SecureRecord* pxRecord)
{
  ALOGV("nvDatabaseSecureTable::selectSecureRecord() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = 0;

  do
    {
      if((rc = sqlite3_prepare_v2(pxDatabase, 
				  gSecureTable._selectQuery, 
				  -1, &pStmt, 0)) != SQLITE_OK)
	{
	  (void)sqlite3_finalize(pStmt);
	  break;
	}
      if((rc = sqlite3_bind_text(pStmt, 1, 
				 xKey, -1, 
				 SQLITE_TRANSIENT)) != SQLITE_OK)
	break;
      if((rc = sqlite3_step(pStmt)) != SQLITE_ROW)
	{
	  (void)sqlite3_finalize(pStmt);
	  break;
	}
      pxRecord->_dataSize = sqlite3_column_bytes(pStmt, 0);
      pxRecord->_data = (unsigned char *)malloc( pxRecord->_dataSize + 1);
      memcpy(pxRecord->_data, sqlite3_column_blob(pStmt, 0), pxRecord->_dataSize);
      pxRecord->_data[pxRecord->_dataSize] = 0;
      ALOGV("Got row: '%s' '%s'\n", xKey, pxRecord->_data);
      rc = sqlite3_finalize(pStmt);
      if (rc != SQLITE_SCHEMA) break;
    }
  while(0);

  ALOGV("nvDatabaseSecureTable::selectSecureRecord() - Exit ");
  return rc;
}

int deleteSecureRecord(sqlite3* pxDatabase, const char* xKey)
{
  ALOGV("nvDatabaseSecureTable::deleteSecureRecord() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = 0;

  do
    {
      if((rc = sqlite3_prepare(pxDatabase, 
			       gSecureTable._deleteQuery, 
			       -1, &pStmt, 0)) != SQLITE_OK)
	break;
      if((rc = sqlite3_bind_text(pStmt, 1, 
				 xKey, -1, 
				 SQLITE_TRANSIENT)) !=SQLITE_OK )
	break;
      if((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
	{
	  (void)sqlite3_finalize(pStmt);
	  break;
	}
      rc = sqlite3_finalize(pStmt);
      if (rc != SQLITE_SCHEMA)
	{
	  // return the number of rows affected
	  rc = sqlite3_changes(pxDatabase);
	  break;
	}
    }
  while(0);

  ALOGV("nvDatabaseSecureTable::deleteSecureRecord() - Exit ");
  return rc;
}
