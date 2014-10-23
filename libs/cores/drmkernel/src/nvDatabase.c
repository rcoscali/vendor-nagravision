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


void databaseError(sqlite3* pxDatabase)
{
  ALOGV("NvDatabase::databaseError() - Enter ");
  if(0 != pxDatabase)
    {
      int errcode = sqlite3_errcode(pxDatabase);
      const char *errmsg = sqlite3_errmsg(pxDatabase);
      ALOGV("Database error %d: %s",errcode, errmsg);
    }
  else
    {
      ALOGV("No database connection");
    }
  ALOGV("NvDatabase::databaseError() - Exit ");
}

int openNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::openNvDatabase() - Enter ");
  int rc = SQLITE_ERROR;

  for(;;)
    {
      if(0 == pxDatabaseConnection)
	{
	  ALOGV("No database connection");
	  break;
	}

      if(strcmp(pxDatabaseConnection->_databaseName,"") == 0)
	{
	  pxDatabaseConnection->_databaseName = "/data/nv.db";
	}

      ALOGV("NvDatabase::openNvDatabase() - Db path: %s", pxDatabaseConnection->_databaseName);

      rc = sqlite3_open(pxDatabaseConnection->_databaseName, &pxDatabaseConnection->_pDatabase);

      if(rc != SQLITE_OK )
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}

      sqlite3_busy_timeout(pxDatabaseConnection->_pDatabase, 5000);
   
      rc = createSecureTable(pxDatabaseConnection->_pDatabase);
      if(rc != SQLITE_OK )
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}
      break;
    }
  ALOGV("NvDatabase::openNvDatabase() - Exit ");
  return rc;
}

int closeNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::closeNvDatabase() - Enter ");
  int rc = SQLITE_ERROR;
  for(;;)
    {
      if(0 == pxDatabaseConnection)
	{
	  ALOGV("No database connection");
	  break;
	}

      if(0 == pxDatabaseConnection->_pDatabase)
	{
	  rc = SQLITE_OK;
	  break;
	}

      rc = sqlite3_close(pxDatabaseConnection->_pDatabase);
      if(rc != SQLITE_OK )
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}

      pxDatabaseConnection->_pDatabase = 0;
      break;
    }
  ALOGV("NvDatabase::closeNvDatabase() - Exit ");
  return rc;
}

int lockNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::lockNvDatabase() - Enter ");
  char* error = 0;
  int rc = SQLITE_ERROR
    ;
  for(;;)
    {
      if(0 == pxDatabaseConnection)
	{
	  ALOGV("No database connection");
	  break;
	}
      if(0 == pxDatabaseConnection->_pDatabase)
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}

      rc = sqlite3_exec(pxDatabaseConnection->_pDatabase,"BEGIN EXCLUSIVE TRANSACTION",0,0,&error);
      if(rc == SQLITE_BUSY)
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}
      break;
    }

  if(0 != error){
    sqlite3_free(error);
  }

  ALOGV("NvDatabase::lockNvDatabase() - Exit ");
  return rc;
}

int unlockNvDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::unLockNvDatabase() - Enter ");
  char* error = 0;
  int rc = SQLITE_ERROR;
  for(;;)
    {
      if(0 == pxDatabaseConnection)
	{
	  ALOGV("No database connection");
	  break;
	}

      if(0 == pxDatabaseConnection->_pDatabase)
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}

      rc = sqlite3_exec(pxDatabaseConnection->_pDatabase,"COMMIT TRANSACTION",0,0,&error);
      if(rc == SQLITE_BUSY)
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  sqlite3_free(error);
	  break;
	}
      break;
    }

  ALOGV("NvDatabase::unLockNvDatabase() - Exit ");
  return rc;
}

int tableExists(const char* xTableName, NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::tableExists() - Enter ");
  int retVal = SQLITE_ERROR;
  const char* exists = "SELECT count(*) from sqlite_master where type='table' and name=";
  char query[100];
  strcpy (query, exists);
  strcat (query,"'");
  strcat (query,xTableName);
  strcat (query,"'");

  sqlite3_stmt *pStmt = 0;
  int rc = SQLITE_ERROR;

  for(;;)
    {
      if(0 == pxDatabaseConnection)
	{
	  ALOGV("No database connection");
	  break;
	}

      if(0 == pxDatabaseConnection->_pDatabase)
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}

      rc = sqlite3_prepare(pxDatabaseConnection->_pDatabase, query, -1, &pStmt, 0);
      if(rc != SQLITE_OK)
	{
	  databaseError(pxDatabaseConnection->_pDatabase);
	  retVal = SQLITE_ERROR;
	  break;
	}

      // Run the virtual machine. We can tell by the SQL statement that
      // at most 1 row will be returned. So call sqlite3_step() once
      // only. Normally, we would keep calling sqlite3_step until it
      // returned something other than SQLITE_ROW.

      rc = sqlite3_step(pStmt);
      if(rc != SQLITE_ROW)
	{
	  (void)sqlite3_finalize(pStmt);
	  retVal = SQLITE_ERROR;
	  databaseError(pxDatabaseConnection->_pDatabase);
	  break;
	}

      int count = atoi(((const char *) sqlite3_column_text(pStmt, 0)));
      if(count > 0)
        {
          retVal = SQLITE_OK;
        }
      // Finalize the statement (this releases resources allocated by
      // sqlite3_prepare() ).
      rc = sqlite3_finalize(pStmt);

      break;

    }

  ALOGV("NvDatabase::tableExists() - Exit ");
  return retVal;
}

int removeDatabase(NvDatabaseConnection* pxDatabaseConnection)
{
  ALOGV("NvDatabase::removeDatabase() - Enter ");
  int retVal = 1;

  for(;;)
    {
      if(SQLITE_OK != closeNvDatabase(pxDatabaseConnection))
	{
	  break;
	}

      retVal = remove(pxDatabaseConnection->_databaseName);

      break;
    }
  ALOGV("NvDatabase::removeDatabase() - Exit ");
  return retVal;
}

int checkDatabasePresence(const char* xDatabasePath)
{
  ALOGV("NvDatabase::checkDatabasePresence() - Enter ");
  int retVal = 1;
  struct stat st;
  retVal = stat(xDatabasePath, &st);
  ALOGV("NvDatabase::checkDatabasePresence() - Exit ");
  return retVal;
}

