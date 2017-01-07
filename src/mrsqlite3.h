/*******************************************************************************
 *
 *                             Messenger Backend
 *                      Copyright (C) 2017 Björn Petersen
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
 * Purpose: MrSqlite3 wraps around SQLite
 *
 *******************************************************************************
 *
 * NB: In general, function names ending with a `__` implie that _no_
 * locking takes place inside the functions!  So the caller must make sure, the
 * database is locked as needed.  Of course, the same is true if you call any
 * sqlite3-function directly.
 *
 ******************************************************************************/


#ifndef __MRSQLITE3_H__
#define __MRSQLITE3_H__
#ifdef __cplusplus
extern "C" {
#endif


/*** library-private **********************************************************/

#include <sqlite3.h>
#include <libetpan/libetpan.h>
#include <pthread.h>
typedef struct mrmailbox_t mrmailbox_t;


/* predefined statements */
enum
{
	 BEGIN_transaction = 0 /* must be first */
	,ROLLBACK_transaction
	,COMMIT_transaction

	,SELECT_v_FROM_config_k
	,INSERT_INTO_config_kv
	,UPDATE_config_vk
	,DELETE_FROM_config_k

	,SELECT_COUNT_FROM_contacts
	,SELECT_naob_FROM_contacts_i
	,SELECT_inao_FROM_contacts_a
	,SELECT_id_FROM_contacts_WHERE_id
	,SELECT_addr_FROM_contacts_WHERE_chat_id
	,SELECT_id_FROM_contacts_WHERE_chat_id
	,SELECT_id_FROM_contacts_ORDER_BY
	,SELECT_id_FROM_contacts_WHERE_query_ORDER_BY
	,SELECT_COUNT_FROM_contacts_WHERE_blocked
	,SELECT_id_FROM_contacts_WHERE_blocked
	,INSERT_INTO_contacts_neo
	,UPDATE_contacts_nao_WHERE_i
	,UPDATE_contacts_SET_origin_WHERE_id
	,UPDATE_contacts_SET_b_WHERE_i
	,DELETE_FROM_contacts_WHERE_id

	,SELECT_COUNT_FROM_chats
	,SELECT_ii_FROM_chats_LEFT_JOIN_msgs
	,SELECT_itndd_FROM_chats_WHERE_i
	,SELECT_id_FROM_chats_WHERE_contact_id
	,UPDATE_chats_SET_draft_WHERE_id
	,UPDATE_chats_SET_n_WHERE_c
	,UPDATE_chats_SET_blocked

	,SELECT_a_FROM_chats_contacts_WHERE_i
	,SELECT_COUNT_FROM_chats_contacts_WHERE_chat_id
	,SELECT_COUNT_FROM_chats_contacts_WHERE_contact_id
	,SELECT_c_FROM_chats_contacts_WHERE_c

	,SELECT_COUNT_FROM_msgs_WHERE_assigned
	,SELECT_COUNT_FROM_msgs_WHERE_unassigned
	,SELECT_COUNT_FROM_msgs_WHERE_state_AND_chat_id
	,SELECT_COUNT_FROM_msgs_WHERE_chat_id
	,SELECT_COUNT_FROM_msgs_WHERE_rfc724_mid
	,SELECT_COUNT_FROM_msgs_WHERE_ft
	,SELECT_COUNT_DISTINCT_f_FROM_msgs_WHERE_c
	,SELECT_i_FROM_msgs_WHERE_ctt
	,SELECT_id_FROM_msgs_WHERE_chat_id_AND_state
	,SELECT_txt_raw_FROM_msgs_WHERE_id
	,SELECT_ircftttstpb_FROM_msg_WHERE_i
	,SELECT_ss_FROM_msgs_WHERE_m
	,SELECT_i_FROM_msgs_LEFT_JOIN_contacts_WHERE_c
	,SELECT_i_FROM_msgs_LEFT_JOIN_contacts_WHERE_unseen
	,SELECT_i_FROM_msgs_WHERE_query
	,INSERT_INTO_msgs_msscftttsmttp
	,INSERT_INTO_msgs_mcftttstpb
	,UPDATE_msgs_SET_chat_id_WHERE_id
	,UPDATE_msgs_SET_state_WHERE_id
	,UPDATE_msgs_SET_state_WHERE_id_AND_state
	,UPDATE_msgs_SET_ss_WHERE_rfc724_mid
	,DELETE_FROM_msgs_WHERE_id
	,DELETE_FROM_msgs_WHERE_rfc724_mid

	,INSERT_INTO_jobs_aafp
	,SELECT_MIN_d_FROM_jobs
	,SELECT_iafp_FROM_jobs
	,DELETE_FROM_jobs_WHERE_id
	,UPDATE_jobs_SET_dp_WHERE_id

	,PREDEFINED_CNT /* must be last */
};


typedef struct mrsqlite3_t
{
	/* prepared statements - this is the favourite way for the caller to use SQLite */
	sqlite3_stmt* m_pd[PREDEFINED_CNT];

	/* m_sqlite is the database given as dbfile to Open() */
	sqlite3*      m_cobj;

	/* helper for MrSqlite3Transaction */
	int           m_transactionCount;

	mrmailbox_t*  m_mailbox;

	/* the user must make sure, only one thread uses sqlite at the same time!
	for this purpose, all calls must be enclosed by a locked m_critical; use mrsqlite3_lock() for this purpose */
	pthread_mutex_t m_critical_;

} mrsqlite3_t;


mrsqlite3_t*  mrsqlite3_new              (mrmailbox_t*);
void          mrsqlite3_unref            (mrsqlite3_t*);
int           mrsqlite3_open__           (mrsqlite3_t*, const char* dbfile);
void          mrsqlite3_close__          (mrsqlite3_t*);
int           mrsqlite3_is_open          (const mrsqlite3_t*);

/* handle configurations, private */
int           mrsqlite3_set_config__     (mrsqlite3_t*, const char* key, const char* value);
int           mrsqlite3_set_config_int__ (mrsqlite3_t*, const char* key, int32_t value);
char*         mrsqlite3_get_config__     (mrsqlite3_t*, const char* key, const char* def); /* the returned string must be free()'d, returns NULL on errors */
int32_t       mrsqlite3_get_config_int__ (mrsqlite3_t*, const char* key, int32_t def);

/* tools, these functions are compatible to the corresponding sqlite3_* functions */
sqlite3_stmt* mrsqlite3_predefine__      (mrsqlite3_t*, size_t idx, const char* sql); /*the result is resetted as needed and must not be freed. CAVE: you must not call this function with different strings for the same index!*/
sqlite3_stmt* mrsqlite3_prepare_v2_      (mrsqlite3_t*, const char* sql); /* the result mus be freed using sqlite3_finalize() */
int           mrsqlite3_execute__        (mrsqlite3_t*, const char* sql);
int           mrsqlite3_table_exists__   (mrsqlite3_t*, const char* name);
void          mrsqlite3_log_error        (mrsqlite3_t*, const char* msg, ...);

/* tools for locking, may be called nested, see also m_critical_ above.
the user of MrSqlite3 must make sure that the MrSqlite3-object is only used by one thread at the same time.
In general, we will lock the hightest level as possible - this avoids deadlocks and massive on/off lockings.
Low-level-functions, eg. the MrSqlite3-methods, do not lock. */
void          mrsqlite3_lock             (mrsqlite3_t*); /* lock or wait; these calls must not be nested in a single thread */
void          mrsqlite3_unlock           (mrsqlite3_t*);

/* nestable transactions, only the outest is really used */
void          mrsqlite3_begin_transaction__(mrsqlite3_t*);
void          mrsqlite3_commit__           (mrsqlite3_t*);
void          mrsqlite3_rollback__         (mrsqlite3_t*);

#ifdef __cplusplus
} /* /extern "C" */
#endif
#endif /* __MRSQLITE3_H__ */

