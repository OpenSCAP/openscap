/**
 * @file   sql.c
 * @brief  sql probe
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <seap.h>
#include <probe-api.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <common/debug_priv.h>
#include <common/bfind.h>
#include <time.h>
#include <errno.h>
#include <opendbx/api.h>
#include "sql.h"

#ifndef SQLPROBE_DEFAULT_CONNTIMEOUT
# define SQLPROBE_DEFAULT_CONNTIMEOUT 30
#endif

typedef struct {
	char *o_engine; /* object engine  */
	char *b_engine; /* backend engine */
} dbEngineMap_t;

static dbEngineMap_t engine_map[] = {
	{ "access",    NULL       },
	{ "db2",       NULL       },
	{ "cache",     NULL       },
	{ "firebird",  "firebird" },
	{ "firstsql",  NULL       },
	{ "foxpro",    NULL       },
	{ "informix",  NULL       },
	{ "ingres",    NULL       },
	{ "interbase", NULL       },
	{ "lightbase", NULL       },
	{ "maxdb",     NULL       },
	{ "monetdb",   NULL       },
	{ "mimer",     NULL       },
	{ "mssql",     "mssql"    }, /* non-standard */
	{ "mysql",     "mysql"    }, /* non-standard */
	{ "oracle",    "oracle"   },
	{ "paradox",   NULL       },
	{ "pervasive", NULL       },
	{ "postgre",   "pgsql"    },
	{ "sqlbase",   NULL       },
	{ "sqlite",    "sqlite"   },
	{ "sqlite3",   "sqlite3"  }, /* non-standard */
	{ "sqlserver", NULL       },
	{ "sybase",    "sybase"   }
};

#define ENGINE_MAP_COUNT (sizeof engine_map / sizeof (dbEngineMap_t))

static int engine_cmp (const char *a, const dbEngineMap_t *b)
{
	if (a == NULL || b == NULL) {
		return -1;
	}
	return strcmp(a, b->o_engine);
}

typedef struct {
	char    *host;
	char    *port;
	char    *user;
	char    *pass;
	char    *db;
	long     conn_timeout;
} dbURIInfo_t;

static void __clearmem(void *ptr, int len)
{
	if (ptr != NULL) {
		do {
			register int       l = len / sizeof(uint32_t) - 1;
			register uint32_t *p = (uint32_t *)ptr;

			while (l >= 0) {
				p[l] = (uint32_t)random();
				--l;
			}
		} while (0);

		do {
			register int      l = len % sizeof(uint32_t);
			register uint8_t *p = (uint8_t *)ptr;

			switch (l) {
			case 3:
				p[2] = (uint8_t)(random() % (1 << 8));
			case 2:
				p[1] = (uint8_t)(random() % (1 << 8));
			case 1:
				p[0] = (uint8_t)(random() % (1 << 8));
			}
		} while (0);
	}
	return;
}

static void dbURIInfo_clear(dbURIInfo_t *ui)
{
	if (ui == NULL)
		return;

	srandom((long)clock()^(long)(ui->pass));

	if (ui->host != NULL) {
		__clearmem(ui->host, strlen(ui->host));
		free(ui->host);
	}

	if (ui->user != NULL) {
		__clearmem(ui->user, strlen(ui->user));
		free(ui->user);
	}

	if (ui->pass != NULL) {
		__clearmem(ui->pass, strlen(ui->pass));
		free(ui->pass);
	}

	if (ui->db != NULL) {
		__clearmem(ui->db, strlen(ui->db));
		free(ui->db);
	}

	ui->host = NULL;
	ui->port = 0;
	ui->user = NULL;
	ui->pass = NULL;
	ui->db   = NULL;

	return;
}

static int dbURIInfo_parse(dbURIInfo_t *info, const char *conn)
{
	char *tmp, *tok, *copy;
	char *conn_copy = strdup(conn);
	copy = conn_copy;
	if (copy == NULL)
		return (-1);

#define skipspace(s) while (isspace(*s)) ++s;

#define matchitem1(tok, first, rest, dst)			\
	case first:						\
		if (strncasecmp((rest), ++(tok), strlen(rest)) == 0) {	\
			tok += strlen(rest);			\
			skipspace(tok);				\
			if (*(tok) != '=') goto __fail;		\
			else (dst) = strdup((tok) + 1);		\
		}						\
		else dE("Unrecognized token: '%s'", (tok)-1);		\
	while(0)

#define matchitem2(tok, first, rest1, dst1, rest2, dst2)		\
	case first:							\
		if (strncasecmp((rest1), (tok)+1, strlen(rest1)) == 0) {	\
			tok += 1+strlen(rest1);				\
			skipspace(tok);					\
			if (*(tok) != '=') goto __fail;			\
			else (dst1) = strdup((tok) + 1);		\
		}							\
		else if (strncasecmp((rest2), (tok)+1, strlen(rest2)) == 0) {	\
			tok += 1+strlen(rest2);				\
			skipspace(tok);					\
			if (*(tok) != '=') goto __fail;			\
			else (dst2) = strdup((tok) + 1);		\
		}							\
		else dE("Unrecognized token: '%s'", (tok));		\
		while(0)

	tmp = NULL;

	while ((tok = strsep (&copy, ";")) != NULL) {
		dI("tok: '%s'.", tok);
		switch (tolower(*tok)) {
			matchitem1(tok, 's',
				  "erver", info->host); break;
			matchitem2(tok, 'p',
				   "ort", info->port,
				   "wd",  info->pass); break;
			matchitem1(tok, 'd',
				   "atabase", info->db); break;
			matchitem1(tok, 'u',
				   "id",info->user); break;
			matchitem1(tok, 'c',
				   "onnecttimeout", tmp);
			if (tmp != NULL) {
				info->conn_timeout = strtol(tmp, NULL, 10);

				if (errno == ERANGE || errno == EINVAL)
					info->conn_timeout = SQLPROBE_DEFAULT_CONNTIMEOUT;

				free(tmp);
			}
			break;

		case '\0':
			break;
		default:
			dE("Unrecognized token: '%s'", (tok));
			break;
		}
	}

	free(conn_copy);
	return (0);
__fail:
	dE("Parsing failed.");
	free(conn_copy);
	return (-1);
}

static int dbSQL_eval(const char *engine, const char *version,
                      const char *conn, const char *sql, probe_ctx *ctx)
{
	int err = -1;
	dbURIInfo_t uriInfo = { .host = NULL,
				.port = 0,
				.user = NULL,
				.pass = NULL,
				.db   = NULL};

	/*
	 * parse the connection string
	 */
	if (dbURIInfo_parse(&uriInfo, conn) != 0) {
		dE("Malformed connection string: %s", conn);
		goto __exit;
	} else {
		int            sql_err = 0;
		odbx_t        *sql_dbh = NULL; /* handle */
		dbEngineMap_t *sql_dbe; /* engine */
		odbx_result_t *sql_dbr; /* result */
		const char    *sql_dbv; /* value */
		SEXP_t        *item;

		sql_dbe = oscap_bfind (engine_map, ENGINE_MAP_COUNT, sizeof(dbEngineMap_t), (char *)engine,
				       (int(*)(void *, void *))&engine_cmp);

		if (sql_dbe == NULL) {
			SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"DB engine not found: %s", engine);
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			err = 0;
			dE("DB engine not found: %s", engine);
			goto __exit;
		}

		if (sql_dbe->b_engine == NULL) {
			SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"DB engine not supported: %s", engine);
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			err = 0;
			dE("DB engine not supported: %s", engine);
			goto __exit;
		}

		int odbx_res = odbx_init (&sql_dbh, sql_dbe->b_engine,
				uriInfo.host, uriInfo.port);
		if (odbx_res != ODBX_ERR_SUCCESS) {
			const char *error_msg = odbx_error(NULL, odbx_res);
			dE("odbx_init failed: e=%s, h=%s:%s msg=%s",
				sql_dbe->b_engine, uriInfo.host, uriInfo.port,
				error_msg != NULL ? error_msg : "(none)");
			if (odbx_res == -ODBX_ERR_NOTEXIST) {
				SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
					"odbx_init failed. Please install the opendbx %s backend", engine);
				probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
				SEXP_free(msg);
				probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
				err = 0;
				fprintf(stderr, "Could not connect to the database. "
					"Please install the opendbx %s backend.\n",
					sql_dbe->b_engine);
			}
			goto __exit;
		}

		/* set options */
		odbx_res = odbx_bind(sql_dbh, uriInfo.db, uriInfo.user, uriInfo.pass, ODBX_BIND_SIMPLE);
		if (odbx_res != ODBX_ERR_SUCCESS)
		{
			const char *error_msg = odbx_error(sql_dbh, odbx_res);
			SEXP_t *msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
				"odbx_bind failed. Could not connect to the database '%s': %s",
				uriInfo.db, error_msg != NULL ? error_msg : "(none)");
			probe_cobj_add_msg(probe_ctx_getresult(ctx), msg);
			SEXP_free(msg);
			probe_cobj_set_flag(probe_ctx_getresult(ctx), SYSCHAR_FLAG_ERROR);
			err = 0;
			dE("odbx_bind failed: db=%s, u=%s, p=%s",
			   uriInfo.db, uriInfo.user, uriInfo.pass);
			odbx_finish(sql_dbh);
			goto __exit;
		}

		if (odbx_query(sql_dbh, sql, strlen (sql)) != ODBX_ERR_SUCCESS) {
			dE("odbx_query failed: q=%s", sql);
			odbx_finish(sql_dbh);

			goto __exit;
		} else {
                        SEXP_t *r0;

			sql_dbr = NULL;
                        item    = probe_item_create(OVAL_INDEPENDENT_SQL, NULL,
                                                    "engine",            OVAL_DATATYPE_STRING, engine,
                                                    "version",           OVAL_DATATYPE_STRING, version,
                                                    "sql",               OVAL_DATATYPE_STRING, sql,
                                                    "connection_string", OVAL_DATATYPE_STRING, conn,
                                                    NULL);

			while ((sql_err = odbx_result (sql_dbh, &sql_dbr, NULL, 0)) == ODBX_RES_ROWS) {
				if (odbx_column_count(sql_dbr) != 1) {
					dE("Don't how to handle result, column count != 1");
					odbx_result_finish(sql_dbr);
					SEXP_free(item);
					goto __exit;
				}

				while (odbx_row_fetch(sql_dbr) == ODBX_ROW_NEXT) {
					sql_dbv = odbx_field_value (sql_dbr, 0);
					/* TODO: support for other "simple" types */
					probe_item_ent_add(item, "result", NULL,
							   r0 = SEXP_string_newf(sql_dbv, strlen(sql_dbv)));
					SEXP_free(r0);
				}

				odbx_result_finish(sql_dbr);
			}

                        probe_item_collect(ctx, item);
		}

		if (odbx_finish(sql_dbh) != ODBX_ERR_SUCCESS) {
			dE("odbx_finish failed");
			goto __exit;
		}
	}

	err = 0;
__exit:
	dbURIInfo_clear(&uriInfo);
	return (err);
}

int sql_probe_main(probe_ctx *ctx, void *arg)
{
	char *engine, *version, *conn, *sqlexp;
	int err;
        SEXP_t *probe_in;

#define get_string(dst, obj, ent_name)					\
	do {								\
		SEXP_t *__sval;						\
									\
		__sval = probe_obj_getentval (obj, ent_name, 1);	\
									\
		if (__sval == NULL) {					\
			dE("Missing entity or value: obj=%p, ent=%s", obj, #ent_name); \
			err = PROBE_ENOENT;				\
			goto __exit;					\
		}							\
									\
		(dst) = SEXP_string_cstr (__sval);			\
									\
		if ((dst) == NULL) {					\
			SEXP_free(__sval);				\
			err = PROBE_EINVAL;				\
			goto __exit;					\
		}							\
									\
		SEXP_free(__sval);					\
	} while (0)

	engine  = NULL;
	version = NULL;
	conn    = NULL;
	sqlexp  = NULL;

        probe_in = probe_ctx_getobject(ctx);

	get_string(engine,  probe_in, "engine");
	get_string(version, probe_in, "version");
	get_string(conn,    probe_in, "connection_string");
	get_string(sqlexp,  probe_in, "sql");

	/*
	 * evaluate the SQL statement
	 */
	err = dbSQL_eval(engine, version, conn, sqlexp, ctx);
__exit:
	if (engine != NULL) {
		__clearmem(conn, strlen(engine));
		free(engine);
	}

	if (sqlexp != NULL) {
		__clearmem(sqlexp, strlen(sqlexp));
		free(sqlexp);
	}

	if (conn != NULL) {
		__clearmem(conn, strlen(conn));
		free(conn);
	}

	if (version != NULL) {
		__clearmem(version, strlen(version));
		free(version);
	}

	return (err);
}
