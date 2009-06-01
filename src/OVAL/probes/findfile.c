

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <mntent.h>
#include <errno.h>
#include <unistd.h>
#include <regex.h>

#include "findfile.h"

typedef struct {
	regex_t re;
	char *direction;
	char *follow;
	dev_t *dev_id_list;
	int dev_id_count;
} setting_t;

typedef struct {
	int pathc;
	char **pathv;
	int offs;
} rglob_t;

static int find_files_recursion(const char* path, setting_t * setting, SEXP_t* files, int depth);
static int init_devs (const char *mtab, char **local, setting_t *setting);
static int match_fs (const char *fs, char **local);
static int recurse_direction(const char *file, char *direction);
static int recurse_filesystem(struct stat *st, dev_t * dev_id_list, int dev_id_count);
static int recurse_follow(struct stat *st, char *follow);
static int noRegex(char * token);
static int rglob(const char *pattern, rglob_t *result);
static void find_paths_recursion(const char *path, regex_t *re, rglob_t *result );
static void rglobfree(rglob_t * result);


/* 
 *
 * ************* PUBLIC *************  
 *
 */
SEXP_t * find_files(SEXP_t * spath, SEXP_t *sfilename, SEXP_t *behaviors) {
	char * local_fs[5] = LOCAL_FILESYSTEMS;
	char *name = NULL, *path = NULL;
	int i, rc, finds;
	int max_depth;
	rglob_t rglobbuf;
	setting_t *setting;	
	SEXP_t *files, *f;

	files = SEXP_list_new();
	name = SEXP_string_cstr(SEXP_OVALelm_getval(sfilename));
	path = SEXP_string_cstr(SEXP_OVALelm_getval(spath));
	SEXP_number_get(SEXP_OVALelm_getattrval(behaviors,"max_depth"),&max_depth, NUM_INT32);

	setting = malloc(sizeof(setting_t));
	setting->direction = SEXP_string_cstr(SEXP_OVALelm_getattrval(behaviors,"recurse_direction"));
	setting->follow = SEXP_string_cstr(SEXP_OVALelm_getattrval(behaviors,"recurse"));

	/* Init list of local devices */
	setting->dev_id_list=NULL;
	if( !SEXP_strncmp(SEXP_OVALelm_getattrval(behaviors,"recurse_file_system"), "local",6) ) {
		if( !init_devs(MTAB_PATH, local_fs, setting) ) {
			printf("Can't init list of local devices\n");
			goto error;
		}
	}

	/* Init pattern match on filename */
	if( regcomp(&(setting->re), name, REG_EXTENDED) != 0 ) {
		printf("Can't init pattern buffer storage area\n");
                goto error;
	}

	/* pattern match on path */
	if( !SEXP_strncmp(SEXP_OVALelm_getattrval(spath,"operation"), "pattern match", 14) ) {
		rglobbuf.offs=10;
		rc = rglob(path, &rglobbuf);
		if(!rc && rglobbuf.pathc > 0) {
			finds = 0;
			for(i=0; i < rglobbuf.pathc; i++) {
				rc = find_files_recursion(rglobbuf.pathv[i], setting, files, max_depth);
				if( !rc && finds == SEXP_length(files) ) { /* add path */
					f = SEXP_list_new();
	                                SEXP_list_add(f,SEXP_string_newf(rglobbuf.pathv[i]));
					SEXP_list_add(files, f);
				}
				finds = SEXP_length(files);
			}
			rglobfree(&rglobbuf);
		}
	}
	else {
		rc = find_files_recursion(path, setting, files, max_depth);
		if( !rc && SEXP_length(files) == 0 ) { /* add path */
			f = SEXP_list_new();
	       		SEXP_list_add(f,SEXP_string_newf(path));
			SEXP_list_add(files, f);
		}	
	}


error:
	if (name) free(name);
	if (path) free(path);

	if (setting->follow) free(setting->follow);
	if (setting->direction) free(setting->direction);
	regfree(&(setting->re));
	if (setting->dev_id_list) free(setting->dev_id_list);
	free(setting);
	
	return files;
}


/* 
 *
 * ************* LOCAL *************  
 *
 */


/*
 *  depth defines how many directories to recurse when a recurse direction is specified
 *  '-1' means no limitation
 *  '0' is equivalent to no recursion
 *  '1' means to step only one directory level up/down
 */
static int find_files_recursion(const char* path, setting_t * setting, SEXP_t *files, int depth) {
	struct stat st;
	struct dirent * pDirent;
	DIR * pDir;
	char * path_new;
	SEXP_t * f;

	pDir = opendir(path);
	if( pDir == NULL) 
		return 1;

	f = SEXP_list_new();
	SEXP_list_add(f,SEXP_string_newf(path));
	while ( (pDirent = readdir(pDir)) ) {
		path_new = malloc( (strlen(path)+1+strlen(pDirent->d_name)+1)*sizeof(char) );
		sprintf(path_new,"%s/%s",path,pDirent->d_name);

		if( lstat(path_new, &st) == -1)
			continue;

		if( recurse_follow(&st,setting->follow) &&                                   /* follow symlinks? */
		    recurse_direction(pDirent->d_name, setting->direction) &&                /* up or down direction? */
		    depth &&					        	             /* how deep rabbit hole is? */
		    recurse_filesystem(&st, setting->dev_id_list, setting->dev_id_count) ) { /* local filesystem? */
			find_files_recursion(path_new, setting, files, depth == - 1 ? -1 : depth - 1);
		}
		else if( !S_ISDIR(st.st_mode) ) {
			/* pattern match on filename*/
			if( regexec(&(setting->re), pDirent->d_name, 0, NULL, 0) == 0 ) {
				SEXP_list_add(f,SEXP_string_newf(pDirent->d_name));
			}
		}

		free(path_new);
	}
	if( SEXP_length(f) > 1 )
		SEXP_list_add(files, f);
	else
		SEXP_free(f);

	closedir(pDir);
	return 0;
}

/*
 * What to follow during recursion?
 * 'directories'
 * 'symlinks'
 * 'both'
 */
static int recurse_follow(struct stat *st, char *follow) {
	
	if( !follow )
		return 1;

	if( !strncmp(follow,"symlinks",9)  && S_ISLNK(st->st_mode) )
		return 1;
		
	if( !strncmp(follow,"directories",12)  && S_ISDIR(st->st_mode) )
		return 1;

	if( !strncmp(follow,"both",5)  && (S_ISDIR(st->st_mode) || S_ISLNK(st->st_mode)) )
		return 1;

	/* deprecated */
	if( !strncmp(follow,"files and directories",12)  && S_ISDIR(st->st_mode)  )
		return 1;

	return 0;
}

/*
 * Up or down? 
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

	return 0;
}

/*
 * Defines the file system limitation 
 * 'local' limiting data collection to local file systems
 * 'defined' to keep any recursion within the file system that the file_object (path+filename) has specified
 */
static int recurse_filesystem(struct stat *st, dev_t * dev_id_list, int dev_id_count) {
	register int i;

	if( dev_id_list == NULL )
		return 1;

	for (i = 0; i < dev_id_count; ++i)
		if( (dev_id_list)[i] == st->st_dev )
			return 1;

	return 0;
}


#define INIT_DEVID_SIZE 64
#define DEVID_GROW_COEF 1.2
static int init_devs (const char *mtab, char **local, setting_t * setting) {
	struct mntent *mnt;
	FILE *fp;
	struct stat st;
	int  DEVid_size;

	fp = setmntent (mtab, "r");
	if (fp == NULL) {
		fprintf (stderr, "setmntent: %s: %s\n", mtab, strerror (errno));
		return 0;
	}

	DEVid_size = INIT_DEVID_SIZE;
	setting->dev_id_list = malloc(sizeof (dev_t) * INIT_DEVID_SIZE);
	setting->dev_id_count = 0;

	while ((mnt = getmntent (fp)) != NULL) {
		if (match_fs (mnt->mnt_type, local)) {
			if (lstat (mnt->mnt_dir, &st) != 0) {
        			fprintf (stderr, "lstat: %s: %s\n", mnt->mnt_dir, strerror (errno));
				return 0;
			}

			setting->dev_id_list[setting->dev_id_count] = st.st_dev;
			(setting->dev_id_count)++;

		        if ( setting->dev_id_count == DEVid_size) {
			        DEVid_size *= DEVID_GROW_COEF;
				setting->dev_id_list =  realloc (setting->dev_id_list, sizeof (dev_t) * DEVid_size );
			}
		}
	}

	fclose (fp);
	return 1;
}

static int match_fs (const char *fs, char **local) {
	register int i = 0;

	while( local[i] ) {
		if (!strcmp (fs, local[i]))
			return 1;
		i++;
	}

	return 0;
}

/*
 * Similar to glob function, but instead off shell wildcard 
 * use regexps 
 */
static int rglob(const char *pattern, rglob_t *result) {
	char * pattern_save,*pattern_tmp;
	char * token;
	char * saveptr;
	char * path;
	regex_t re;

	/* init result */
	if ( result->offs < 1 || result->offs > 1000 )
		result->offs=10;
	result->pathv = malloc( sizeof (char**) * result->offs);
	result->pathc=0;

	/* get no regexp portion */
	path = malloc( sizeof(char *) * ((strlen(pattern))+1) );
	path[0] = '\0';
	pattern_save = strdup(pattern);
	pattern_tmp = pattern_save;
	saveptr=NULL;
	for (; ;pattern_save=NULL) {
		token = strtok_r(pattern_save, "/", &saveptr);
		if( token==NULL)
			break;
		if( noRegex(token) )
			sprintf(path+strlen(path),"/%s",token);
		else
			break;
	}
	free(pattern_tmp);

	/* is there a $ at the end of the pattern? Add it if not!!!  */
	pattern_save = malloc(sizeof(char*)*strlen(pattern) + 2);
	if( pattern[strlen(pattern)-1] != '$')
		sprintf(pattern_save,"%s$",pattern);
	else
		sprintf(pattern_save,"%s",pattern);

	/* init regex machine */
	 if( regcomp(&re, pattern_save, REG_EXTENDED) != 0 ) {
                printf("Can't init pattern buffer storage area\n");
                return 1;
        }

	/* find paths */
	find_paths_recursion(path, &re, result );

	free(pattern_save);
	regfree(&re);
	free(path);	
	return 0;
	
}

static void find_paths_recursion(const char *path, regex_t *re, rglob_t *result ) {
        struct dirent * pDirent;
	struct stat st;
        DIR * pDir;
	char * path_new;

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
                path_new = malloc( (strlen(path)+1+strlen(pDirent->d_name)+1)*sizeof(char) );
                sprintf(path_new,"%s/%s",path,pDirent->d_name);

                if( lstat(path_new, &st) == -1)
                        continue;

		if( S_ISDIR(st.st_mode) && strncmp(pDirent->d_name,"..",3) && strncmp(pDirent->d_name,".",2) ) {
			find_paths_recursion(path_new, re, result ); /* recursion */
		}
		free(path_new);
	}
	closedir(pDir);
}

static int noRegex(char * token) {
	char regexChars[] = "^$\\.[](){}*+?";
	size_t i,j;
	int rc;
	
	rc = 1;
	for(i=0; rc && i<strlen(token); i++) {
		for(j=0; rc && j<strlen(regexChars); j++) {
			if( token[i] == regexChars[j] ) {
					rc=0;
			}
		}
	}

	return rc;
}

static void rglobfree(rglob_t * result) {
	int i;

	for(i=0; i<result->pathc; i++) {
		/*printf("result->pathv[%d]: %s\n", i, result->pathv[i]);*/
		free(result->pathv[i]);
	}
	free(result->pathv);

	result->pathc=0;
}

