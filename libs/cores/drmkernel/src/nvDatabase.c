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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define LOG_NDEBUG 0
#define LOG_TAG "nvDatabase"
#include <utils/Log.h>

#include "sqlite3.h"
#include "nvDatabase.h"
#include "nvDatabaseSecureTable.h"

static const char * mNvDatabasePathname = NV_DATABASE_PATHNAME;

const char *
getNvDatabasePathname(void)
{
  return mNvDatabasePathname;
}

void databaseError(sqlite3* pxDatabase)
{
  ALOGV("NvDatabase::databaseError() - Enter ");

  if(pxDatabase)
    {
      int errcode = sqlite3_errcode(pxDatabase);
      const char *errmsg = sqlite3_errmsg(pxDatabase);
      ALOGE("Database error %d: %s",errcode, errmsg);
    }
  else
    ALOGV("No database connection");

  ALOGV("NvDatabase::databaseError() - Exit ");
}

int openNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::openNvDatabase() - Enter ");
  int rc = SQLITE_ERROR;

  if(!pxDatabaseConnection)
    ALOGE("No database connection");

  else
    do
      {
	if(!pxDatabaseConnection->_databaseName || 
	   !strcmp(pxDatabaseConnection->_databaseName,""))
	  pxDatabaseConnection->_databaseName = mNvDatabasePathname;
	
	ALOGV("NvDatabase::openNvDatabase() - Db path: %s", 
	      pxDatabaseConnection->_databaseName);
	
	if((rc = sqlite3_open(pxDatabaseConnection->_databaseName, 
			      &pxDatabaseConnection->_pDatabase) )!= SQLITE_OK )
	  {
	    databaseError(pxDatabaseConnection->_pDatabase);
	    break;
	  }
	
	sqlite3_busy_timeout(pxDatabaseConnection->_pDatabase, 5000);
	
	if((rc = createSecureTable(pxDatabaseConnection->_pDatabase) )!= SQLITE_OK )
	  {
	    databaseError(pxDatabaseConnection->_pDatabase);
	    break;
	  }
      }
    while(0);

  ALOGV("NvDatabase::openNvDatabase() - Exit ");
  return rc;
}

int closeNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::closeNvDatabase() - Enter ");
  int rc = SQLITE_ERROR;

  if(!pxDatabaseConnection || !pxDatabaseConnection->_pDatabase)
    {
      ALOGE("No database connection");
      rc = SQLITE_OK;
    }
  else
    do
      {
	if((rc = sqlite3_close(pxDatabaseConnection->_pDatabase)) != SQLITE_OK )
	  {
	    databaseError(pxDatabaseConnection->_pDatabase);
	    break;
	  }
	
	pxDatabaseConnection->_pDatabase = 0;
      }
    while(0);

  ALOGV("NvDatabase::closeNvDatabase() - Exit ");
  return rc;
}

int lockNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::lockNvDatabase() - Enter ");
  char* error = 0;
  int rc = SQLITE_ERROR;

  if(!pxDatabaseConnection || !pxDatabaseConnection->_pDatabase)
    {
      ALOGE("No database connection");
      if (pxDatabaseConnection) databaseError(pxDatabaseConnection->_pDatabase);
    }
  else
    do
      {
	if((rc = sqlite3_exec(pxDatabaseConnection->_pDatabase,
			      "BEGIN EXCLUSIVE TRANSACTION",
			      0,0,&error))== SQLITE_BUSY)
	  {
	    databaseError(pxDatabaseConnection->_pDatabase);
	    break;
	  }
      }
    while(0);
  
  if(error)
    sqlite3_free(error);

  ALOGV("NvDatabase::lockNvDatabase() - Exit ");
  return rc;
}

int unlockNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::unLockNvDatabase() - Enter ");
  char* error = 0;
  int rc = SQLITE_ERROR;

  if(!pxDatabaseConnection || !pxDatabaseConnection->_pDatabase)
    {
      ALOGE("No database connection");
      if (pxDatabaseConnection) databaseError(pxDatabaseConnection->_pDatabase);
    }
  else
    if((rc = sqlite3_exec(pxDatabaseConnection->_pDatabase,
			  "COMMIT TRANSACTION",
			  0,0,&error)) == SQLITE_BUSY)
      {
	databaseError(pxDatabaseConnection->_pDatabase);
	sqlite3_free(error);
      }

  ALOGV("NvDatabase::unLockNvDatabase() - Exit (%d)\n", rc);
  return rc;
}

int tableExists(const char* xTableName, NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::tableExists() - Enter ");
  sqlite3_stmt *pStmt = 0;
  int rc = SQLITE_ERROR;

  if(!pxDatabaseConnection || !pxDatabaseConnection->_pDatabase)
    {
      ALOGE("No database connection !!");
      if (pxDatabaseConnection) databaseError(pxDatabaseConnection->_pDatabase);
    }
  else
    do
      {
	if((rc = sqlite3_prepare_v2(pxDatabaseConnection->_pDatabase, 
				    "SELECT count(*) from sqlite_master "
				    "where "
				    "type='table' and name=?", 
				    -1, 
				    &pStmt, 0) )!= SQLITE_OK)
	  {
	    databaseError(pxDatabaseConnection->_pDatabase);
	    rc = SQLITE_ERROR;
	    break;
	  }

	if((rc = sqlite3_bind_text(pStmt, 1, xTableName, -1, SQLITE_TRANSIENT))!= SQLITE_OK )
	  break;

	rc = sqlite3_step(pStmt);
	if(rc != SQLITE_ROW)
	  {
	    (void)sqlite3_finalize(pStmt);
	    rc = SQLITE_ERROR;
	    databaseError(pxDatabaseConnection->_pDatabase);
	    break;
	  }

	int count = atoi(((const char *)sqlite3_column_text(pStmt, 0)));
	if(count > 0)
	  rc = SQLITE_OK;
	rc = sqlite3_finalize(pStmt);
      }
    while(0);

  ALOGV("NvDatabase::tableExists() - Exit (%d)\n", rc);
  return rc;
}

int removeDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::removeDatabase() - Enter ");
  int retVal = 1;

  if(closeNvDatabase(pxDatabaseConnection) == SQLITE_OK)
    retVal = remove(pxDatabaseConnection->_databaseName);

  ALOGV("NvDatabase::removeDatabase() - Exit (%d)\n", retVal);
  return retVal;
}

int checkDatabasePresence(const char* xDatabasePath)
{
  ALOGV("NvDatabase::checkDatabasePresence() - Enter ");
  int retVal = -1;

  retVal = access(xDatabasePath, R_OK|W_OK);

  ALOGV("NvDatabase::checkDatabasePresence() - Exit (%d)\n", retVal);
  return retVal;
}

