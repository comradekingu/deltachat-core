/*******************************************************************************
 *
 *                              Delta Chat Core
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
 * File:    mrkey.h
 * Purpose: Handle keys
 *
 ******************************************************************************/


#ifndef __MRKEY_H__
#define __MRKEY_H__
#ifdef __cplusplus
extern "C" {
#endif


/*** library-private **********************************************************/

typedef struct mrmailbox_t mrmailbox_t;


#define MR_PUBLIC  0
#define MR_PRIVATE 1


typedef struct mrkey_t
{
	int            _m_heap_refcnt; /* !=0 for objects created with mrkey_new(), 0 for stack objects  */

	void*          m_binary;
	int            m_bytes;
	int            m_type;
} mrkey_t;


mrkey_t* mrkey_new           ();
mrkey_t* mrkey_ref           (mrkey_t*);
void     mrkey_unref         (mrkey_t*);

int   mrkey_set_from_raw  (mrkey_t*, const void* data, int bytes, int type);
int   mrkey_set_from_key  (mrkey_t*, const mrkey_t*);
int   mrkey_set_from_stmt (mrkey_t*, sqlite3_stmt*, int index, int type);
int   mrkey_set_from_base64(mrkey_t*,const char* base64, int type);
int   mrkey_set_from_file (mrkey_t*, const char* file, mrmailbox_t* mailbox);

int   mrkey_equals        (const mrkey_t*, const mrkey_t*);

int   mrkey_save_self_keypair__(const mrkey_t* public_key, const mrkey_t* private_key, const char* addr, int is_default, mrsqlite3_t* sql);
int   mrkey_load_self_public__ (mrkey_t*, const char* self_addr, mrsqlite3_t* sql);
int   mrkey_load_self_private_for_signing__(mrkey_t*, const char* self_addr, mrsqlite3_t* sql);

char* mr_render_base64   (const void* buf, size_t buf_bytes, int break_every, const char* break_chars, int add_checksum); /* the result must be freed */
char* mrkey_render_base64(const mrkey_t* ths, int break_every, const char* break_chars, int add_checksum); /* the result must be freed */
char* mrkey_render_asc   (const mrkey_t*); /* the result must be freed */
char* mrkey_render_fingerprint(const mrkey_t*, mrmailbox_t* mailbox);
char* mr_render_fingerprint(const uint8_t* data, size_t bytes);
void  mr_wipe_secret_mem(void* buf, size_t buf_bytes);


#ifdef __cplusplus
} /* /extern "C" */
#endif
#endif /* __MRKEY_H__ */

