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

#ifndef _NVDATABASESECURETABLE_H
#define _NVDATABASESECURETABLE_H

#include <string.h>

#include "sqlite3.h"

/**
 *@brief
 *  Define the table schema of the secure table
 */
typedef struct SecureRecordSchema 
{
  // name of the table
  const char* _tableName;
  // the name of the key column
  const char* _keyColumn;
  // create table query
  const char* _createQuery;
  // insert record query
  const char* _insertQuery;
  // select record query
  const char* _selectQuery;
  //delete record query
  const char* _deleteQuery;
  //get key by keyid record query
  const char* _keyByKeyidQuery;
} SecureRecordSchema;

static SecureRecordSchema gSecureTable =
{
  "Secure",
  "KEY",
  "CREATE TABLE IF NOT EXISTS Secure(KEY TEXT NOT NULL, KID BLOB, KCLEN INTEGER, THEKC BLOB, TAGLEN INTEGER, UNTAG BLOB);",
  "INSERT INTO Secure(KEY, KID, KCLEN, THEKC, TAGLEN, UNTAG) VALUES(?1, ?2, ?3, ?4, ?5, ?6)",
  "SELECT KEY, KID, KCLEN, THEKC, TAGLEN, UNTAG FROM Secure WHERE KEY=?1",
  "DELETE FROM Secure WHERE KEY=?1",
  "SELECT KEY, KID, KCLEN, THEKC, TAGLEN, UNTAG FROM Secure WHERE KID=?1",
};

/**
 *@brief
 *  Define the secure record structure
 */
typedef struct SecureRecord 
{
  // The key for the record
  const char* _key;
  // ID of key used for content protection
  unsigned char* _keyId;
  // The corresponding data associated with the key
  // memory should be freed by the caller
  unsigned char* _contentKey;
  // The size in bytes of the data in the record
  unsigned int _contentKeySize;
  // The corresponding tag associated with the key
  // memory should be freed by the caller
  unsigned char* _tag;
  // The size in bytes of the authen tag in the record
  unsigned int _tagSize;
} SecureRecord;


/**
 *@brief
 *  Insert a record into the table,
 *  if the record already exists it 
 *  is deleted and replaced by the new insert.
 *@param [in] pxDatabase
 *    a pointer to the database connection
 *@pram [in] pxRecord
 *  A pointer to the structure containing the record to be inserted
 *@return SQLITE_OK
 *  The record was inserted succesfully
 *  otherwise the record was not inserted
 */
int insertSecureRecord(sqlite3* pxDatabase, SecureRecord *pxRecord);

/**
 *@brief
 *  Create a secure table in the database
 *@param [in] pxDatabase
 *    a pointer to the database connection
 *@return SQLITE_OK
 *  The record was insertetable was created succesfully
 *  otherwise the record was not insertedtable was not created
 */
int createSecureTable(sqlite3* pxDatabase);

/**
 *@brief
 *  Select a record from the table,
 *@param [in] pxDatabase
 *    a pointer to the database connection
 *@param [in] xKey
 *    The key of the record to select
 *@pram [out] xRecord
 *  A pointer to the structure containing the record retrieved
 *@return SQLITE_OK
 *  The record was retrieved succesfully
 *  otherwise the record was not retrieved
 */
int selectSecureRecord(sqlite3* pxDatabase, const char* xKey, const uint8_t *xKeyId, SecureRecord* pxRecord);

/**
 *@brief
 *  Delete a record from the table,
 *@param [in] pxDatabase
 *    a pointer to the database connection
 *@param [in] xKey
 *    The key of the record to delete
 *@return SQLITE_OK
 *  The record was deleted succesfully
 *  otherwise the record was not deleted
 */
int deleteSecureRecord(sqlite3* pxDatabase, const char* xKey);


#endif /* _NVDATABASESECURETABLE_H */
