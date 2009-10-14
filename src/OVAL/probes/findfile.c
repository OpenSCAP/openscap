#ifndef __STUB_PROBE
/*! \file findfile.c
 *  \brief auxiliary find_files() function used in different probes
 *
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      Peter Vrabec <pvrabec@redhat.com>
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <regex.h>
#include <assert.h>
#include <common/alloc.h>

#include "findfile.h"

typedef struct {
	regex_t *re;
	char *file;
	char *direction;
	char *follow;
        fsdev_t *dev_list;
	int (*cb) (const char * pathname, const char *filename, void *arg);
} setting_t;

typedef struct {
	int pathc;
	char **pathv;
	int offs;
} rglob_t;

static int find_files_recursion(const char* path, setting_t * setting, int depth, void *arg );
static int recurse_direction(const char *file, char *direction);
static int recurse_follow(struct stat *st, char *follow);
static int noRegex(char * token);
static int rglob(const char *pattern, rglob_t *result);
static void find_paths_recursion(const char *path, regex_t *re, rglob_t *result );
static void rglobfree(rglob_t * result);


/* ************************************* PUBLIC ***********************************  */

/*  Collect files and paths according to defined behavior. This function can be used in 
 *  these probes: file_object, textfilecontent54
 *  
 *
 *  @spath  The path to a file on the machine, not including the filename
 *  @sfilename The name of a file
 *  @behaviors Specify find_files settings
 *  @cb Callback that is called for each hit
 */
int find_files(SEXP_t * spath, SEXP_t *sfilename, SEXP_t *behaviors,
               int (*cb) (const char *pathname, const char *filename, void *arg), void *arg) {
	char *name = NULL, *path = NULL, *tmp = NULL;
	setting_t *setting = NULL;	
	int i, rc, max_depth;
	rglob_t rglobbuf;
	int finds = 0;
        SEXP_t *stmp;

	assert(sfilename);
	assert(spath);
	assert(behaviors);

	name = SEXP_string_cstr(probe_ent_getval(sfilename));
	path = SEXP_string_cstr(probe_ent_getval(spath));
	tmp  = SEXP_string_cstr(probe_ent_getattrval(behaviors,"max_depth"));
	max_depth = atoi(tmp);
	oscap_free(tmp);

	setting = calloc(1, sizeof(setting_t));
	setting->direction = SEXP_string_cstr(probe_ent_getattrval(behaviors,"recurse_direction"));
	setting->follow = SEXP_string_cstr(probe_ent_getattrval(behaviors,"recurse"));
	setting->cb = cb;

	/* Init list of local devices */
	setting->dev_list = NULL;
	stmp = probe_ent_getattrval(behaviors,"recurse_file_system");
	if( stmp && !SEXP_strncmp(stmp, "local",6) ) {
                if ((setting->dev_list = fsdev_init (NULL, 0)) == NULL) {
			goto error;
		}
	}
        
	/* Filename */
	setting->re = NULL;
	setting->file = name;
	if( name ) {
		if( SEXP_number_getu_32 (probe_ent_getattrval(sfilename,"operation")) ==  OVAL_OPERATION_PATTERN_MATCH ) {
			setting->re = malloc(sizeof(regex_t));
			if( regcomp(setting->re, name, REG_EXTENDED) != 0 ) {
		       	        goto error;
			}
		}
	}

        assert (strlen (path) > 0);

	/* Is there a '/' at the end of the path? */
	i = strlen(path)-1;

	if( path[i] == '/' && i > 0)
		path[i] = '\0';

        /* Evaluate path(s) */
	if( SEXP_number_getu_32 (probe_ent_getattrval(spath,"operation")) ==  OVAL_OPERATION_PATTERN_MATCH ) {
		rglobbuf.offs=10;
		rc = rglob(path, &rglobbuf);
		if(!rc && rglobbuf.pathc > 0) {
			finds = 0;
			for(i=0; i < rglobbuf.pathc; i++) {
				rc = find_files_recursion(rglobbuf.pathv[i], setting, max_depth, arg);
				if( rc == 0 ) {/* add path, no files found*/
					(*cb)(rglobbuf.pathv[i], NULL, arg);
					rc++;
				}
				if( rc >= 0 )
					finds += rc;
			}
			rglobfree(&rglobbuf);
		}
	}
	else {
		rc = find_files_recursion(path, setting, max_depth, arg);
		if( rc == 0 ) { /* add path, no files found */
			(*cb)(path, NULL, arg);
			rc++;
		}
		if( rc >= 0 )
			finds += rc;
	}


error:
	oscap_free(name);  /* setting->file is same adress*/
	oscap_free(path);

	oscap_free(setting->follow);
	oscap_free(setting->direction);
	fsdev_free (setting->dev_list);
	if (setting->re != NULL)
		regfree(setting->re);
	oscap_free(setting);
	
	return finds;
}


/* ********************************* PRIVATE *******************************  */


/*
 *  depth defines how many directories to recurse when a recurse direction is specified
 *  '-1' means no limitation
 *  '0' is equivalent to no recursion
 *  '1' means to step only one directory level up/down
 */
static int find_files_recursion(const char* path, setting_t * setting, int depth, void *arg ) {
	struct stat st;
	struct dirent * pDirent;
	char path_new[PATH_MAX];
	DIR * pDir;
	int rc, tmp;

	pDir = opendir(path);
	if( pDir == NULL) 
		return -1;

	rc = 0;
	while ( (pDirent = readdir(pDir)) ) {
		if (PATH_MAX < strlen(path) + 1 + pDirent->d_reclen + 1)
			continue;
		sprintf(path_new,"%s/%s",path,pDirent->d_name);

		if( lstat(path_new, &st) == -1)
			continue;

		if( depth &&                                                   /* how deep rabbit hole is? */
		    recurse_direction(pDirent->d_name, setting->direction) &&  /* up or down direction? */
		    recurse_follow(&st,setting->follow) &&                     /* follow symlinks? */
		    fsdev_search (setting->dev_list, &st.st_dev)) {            /* local filesystem? */
			tmp = find_files_recursion(path_new, setting, depth == - 1 ? -1 : depth - 1, arg);
			if( tmp >=0 )
				rc += tmp;
		}

		if( !S_ISDIR(st.st_mode) ) {
			if( setting->re ) { /* patter match */
				if( regexec(setting->re, pDirent->d_name, 0, NULL, 0) == 0 ) {
					rc++;
					(setting->cb)(path, pDirent->d_name, arg);
				}
			} else if ( setting->file ) { /* filename match */
				if(!strncmp(setting->file, pDirent->d_name, pDirent->d_reclen)) {
					rc++;
					(setting->cb)(path, pDirent->d_name, arg);
				}
			}
		} else if( !setting->re && !setting->file ) { /*  collect directories */
			rc++;
			(setting->cb)(path, pDirent->d_name, arg);
		}
	}

	closedir(pDir);
	return rc;
}

/*
 * What to follow during recursion?
 * 'symlinks and directories' --default
 * 'directories'
 * 'symlinks'
 */
static int recurse_follow(struct stat *st, char *follow) {
	
	if ( !(S_ISDIR(st->st_mode) || S_ISLNK(st->st_mode)) )
		return 0;

	/* default */
	if( (!follow || !strncmp(follow, "symlinks and directories",24)) )
		return 1;

	/* deprecated */
	if( !strncmp(follow,"files and directories",12)  && S_ISDIR(st->st_mode)  )
		return 1;

	/* one or another, not very usual case */
	if( !strncmp(follow,"symlinks",9)  && S_ISLNK(st->st_mode) )
		return 1;	
	if( !strncmp(follow,"directories",12)  && S_ISDIR(st->st_mode) )
		return 1;

	return 0;
}

/*
 * Up or down? 
 * 'none' - default
 * 'up' to parent directories 
 * 'down' into child directories
 */
 
static int recurse_direction(const char *file, char *direction) {

	if (!strncmp(direction,"down",5)) {
		if( strncmp(file,"..",3) && strncmp(file,".",2) )
			return 1;
	}
	else if (!strncmp(direction,"up",3)) {
		if( !strncmp(file,"..",3) )
			return 1;
	}
	/* default none*/
	return 0;
}

/*
 * Similar to glob function, but instead off shell wildcard 
 * use regexps 
 */
static int rglob(const char *pattern, rglob_t *result) {
	char * tmp, * token, * saveptr;
	char path[PATH_MAX] = "/";
	int len = 1;
	regex_t re;

	/* check pattern */
	if( !pattern || pattern[0]!='/')
		return 1;

	/* get no regexp portion from pattern to path*/
	tmp = strdup (pattern);
	for ( ; ; tmp=NULL) {
		token = strtok_r(tmp, "/", &saveptr);
		if( token==NULL)
			break;
		if( noRegex(token) ) {
			strcat(path,token);
			strcat(path,"/");
		} else
			break;
	}
	free(tmp);
	/* erase last slash, but not the first one! */
	len = strlen(path);
	if ( len > 1 )
		path[len-1] = '\0';


	/* init regex machine */
	 if( regcomp(&re, pattern, REG_EXTENDED) != 0 ) 
                return 1; /* Can't init pattern buffer storage area */

	/* allocate memory for result */
	if ( result->offs < 1 || result->offs > 1000 )
		result->offs=10;
	result->pathv = malloc( sizeof (char**) * result->offs);
	result->pathc=0;

	/* find paths */
	find_paths_recursion(path, &re, result );

	regfree(&re);
	return 0;
	
}

static void find_paths_recursion(const char *path, regex_t *re, rglob_t *result ) {
        struct dirent * pDirent;
	struct stat st;
        DIR * pDir;
	char path_new[PATH_MAX];

        pDir = opendir(path);
        if( pDir == NULL)
                return;

	if( regexec(re, path, 0, NULL, 0) == 0 ) { /* catch? */
		result->pathv[result->pathc] = strdup(path);
		result->pathc++;
		if( result->pathc == result->offs) {
			result->offs = result->offs * 2; /* magic constant */
			result->pathv = realloc(result->pathv, sizeof (char**) * result->offs);
		}
	}

	while ( (pDirent = readdir (pDir)) ) {
		if (PATH_MAX < strlen(path) + 1 + pDirent->d_reclen + 1)
			continue;
		sprintf(path_new,"%s/%s",path,pDirent->d_name);

                if( lstat(path_new, &st) == -1)
                        continue;

		if( S_ISDIR(st.st_mode) && strncmp(pDirent->d_name,"..",3) && strncmp(pDirent->d_name,".",2) ) {
			find_paths_recursion(path_new, re, result ); /* recursion */
		}
	}
	closedir(pDir);
}

static int noRegex(char * token) {
	const char regexChars[] = "^$\\.[](){}*+?";
	size_t i,token_len;
	
	token_len = strlen(token);
	for(i=0; i<token_len; i++) {
		if( strchr(regexChars, token[i]) )
			return 0;
	}
	return 1;
}

static void rglobfree(rglob_t * result) {
	int i;

	for(i=0; i<result->pathc; i++) {
		free(result->pathv[i]);
	}
	free(result->pathv);

	result->pathc=0;
}

#endif
