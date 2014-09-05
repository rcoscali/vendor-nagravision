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

#ifndef _NVDATABASE_H
#define _NVDATABASE_H

#include "sqlite3.h"

/**
 *@brief
 *  Deifine the database connection structure
 */
typedef struct NvDatabaseConnection 
{
  // The path to the database
  const char* _databaseName;
  // The sqlite database connection
  sqlite3* _pDatabase;
} NvDatabaseConnection;

/**
 *@brief
 *  Create a connection to the database
 *
 *@param [out] pxDatabaseConnection
 *   The database connection
 *
 *@return SQLITE_OK
 *  The connection was created succesfully
 *  otherwise the connection was not created
 */
int openNvDatabase(NvDatabaseConnection* pxDatabaseConnection);

/**
 *@brief
 *   Close the connection to the database
 *
 *@param [out] pxDatabaseConnection
 *   The database connection
 *
 *@return SQLITE_OK
 *  The connection was closed successfully
 *  otherwise the connection was not closed
 */
int closeNvDatabase(NvDatabaseConnection* pxDatabaseConnection);

/**
 *@brief
 *   Lock the database to begin a transaction
 *
 *@param  [in] pxDatabaseConnection
 *    The database connection
 *
 *@return SQLITE_OK
 *  The database was locked for transaction processing
 *  otherwise the database could not be locked
 */
int lockNvDatabase(NvDatabaseConnection* pxDatabaseConnection);


/**
 *@brief
 *   Unlock the database, commit transaction
 *
 *@param  [in] pxDatabaseConnection
 *    The database connection
 *
 *@return SQLITE_OK
 *  The database was unlocked, transaction was commited 
 *  otherwise the database could not be unlocked
 */
int unlockNvDatabase(NvDatabaseConnection* pxDatabaseConnection);

/**
 *@brief
 *   Check if a table exists in the database
 *
 *@param xTableName
 *    The name of the table to check in the db
 *
 *@param  [in] pxDatabaseConnection
 *    The database connection
 *
 *@retval SQLITE_OK
 *  The table exists in the database
 *@retval SQLITE_ERROR
 *  The table does not exist in the database
 */
int tableExists(const char* xTableName, NvDatabaseConnection* pxDatabaseConnection);

/**
 *@brief
 *   Check if a table exists in the database
 *
 *@param [in] pxDatabaseConnection
 *   The database connection
 *
 *@retval 0
 *  The database was removed
 *@retval 1
 *  The database was not removed
 **/
int removeDatabase(NvDatabaseConnection* pxDatabaseConnection);

/**
 *@brief
 *   Check if a a database is present
 *
 *@param [in] xDatabasePath
 *    The database path
 *
 *@retval 0
 *  The database is present
 *  otherwise
 *  The database is not present
 **/
int checkDatabasePresence(const char* xDatabasePath);


#endif /* _NVDATABASE_H */
