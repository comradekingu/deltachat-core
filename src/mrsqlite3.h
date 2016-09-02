/*******************************************************************************
 *
 *                             Messenger Backend
 *     Copyright (C) 2016 Björn Petersen Software Design and Development
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 *******************************************************************************
 *
 * File:    mrsqlite3.h
 * Authors: Björn Petersen
 * Purpose: MrSqlite3 wraps around SQLite
 *
 ******************************************************************************/


#ifndef __MRSQLITE3_H__
#define __MRSQLITE3_H__


#include <sqlite3.h>
#include <libetpan.h>
#include <pthread.h>
#include "mrchat.h"


class MrMailbox;
class MrContact;


// predefined statements
enum
{
	 BEGIN_transaction = 0 // must be first
	,ROLLBACK_transaction
	,COMMIT_transaction

	,SELECT_value_FROM_config_k
	,INSERT_INTO_config_kv
	,UPDATE_config_vk
	,DELETE_FROM_config_k

	,SELECT_COUNT_FROM_contacts
	,SELECT_FROM_contacts_e
	,INSERT_INTO_contacts_ne
	,UPDATE_contacts_ni

	,SELECT_COUNT_FROM_chats

	,SELECT_COUNT_FROM_msg
	,SELECT_id_FROM_msg_m
	,INSERT_INTO_msg_mcfttsmm
	,INSERT_INTO_msg_to_mc

	,PREDEFINED_CNT // must be last
};


class MrSqlite3
{
public:
	              MrSqlite3            (MrMailbox*);
	              ~MrSqlite3           ();
	bool          Open                 (const char* dbfile);
	void          Close                ();
	bool          Ok                   () const { return (m_cobj!=NULL); }

	// handle configurations
	bool          SetConfig            (const char* key, const char* value);
	bool          SetConfigInt         (const char* key, int32_t value);
	char*         GetConfig            (const char* key, const char* def); // the returned string must be free()'d, returns NULL on errors
	int32_t       GetConfigInt         (const char* key, int32_t def);

	// prepared statements - this is the favourite way for the caller to use SQLite
	sqlite3_stmt* m_pd[PREDEFINED_CNT];

	// the caller must make sure, only one thread uses sqlite at the same time!
	// for this purpose, all calls must be enclosed by a locked m_critical - to simplify this, you can alse use MrSqlite3Locker
	pthread_mutex_t m_critical;

	// m_sqlite is the database given as dbfile to Open()
	char*         m_dbfile; // may be NULL
	sqlite3*      m_cobj;

	// helper for MrSqlite3Transaction
	int           m_transactionCount;

	// tools, these functions are compatible to the corresponding sqlite3_* functions
	sqlite3_stmt* sqlite3_prepare_v2_  (const char* sql); // the result mus be freed using sqlite3_finalize()
	bool          sqlite3_execute_     (const char* sql);
	bool          sqlite3_table_exists_(const char* name);

private:
	MrMailbox*    m_mailbox;
};


class MrSqlite3Locker
{
public:
	// the user of MrSqlite3 must make sure that the MrSqlite3-object is only used by one thread at the same time.
	// for this purpose, he can use MrSqlite3Locker as a helper:
	// By the simple existance of a object, all other object creation will be halted until the first is deleted again.
	//
	// In general, we will lock the hightest level as possible - this avoids deadlocks and massive on/off lockings.
	// Low-level-functions, eg. the MrSqlite3-methods, do not lock.
	MrSqlite3Locker(MrSqlite3& sqlite3)
	{
		m_sqlite3 = &sqlite3;
		pthread_mutex_lock(&m_sqlite3->m_critical);
	}

	~MrSqlite3Locker()
	{
		pthread_mutex_unlock(&m_sqlite3->m_critical);
	}

private:
	MrSqlite3* m_sqlite3;
};


class MrSqlite3Transaction
{
public:
	// Transaction objects may be nested, however, only the outest
	// object is really used.  Without an explicit Commit(), the transaction is
	// ROLLBACK'ed on destruction of the outest transaction object.
	//
	// CAVE: As all sqlite requests, transactions should always be locked!
	//
	// NB: While transactions are useful for data integrity, on sqlite,
	// they will also speed up inserts and updates massively!
	                MrSqlite3Transaction   (MrSqlite3& sqlite3);
	                ~MrSqlite3Transaction  ();
	bool            Commit                 ();

private:
	MrSqlite3*      m_sqlite3;
	bool            m_commited;
};


#endif // __MRSQLITE3_H__

