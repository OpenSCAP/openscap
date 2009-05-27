

#include <stdio.h>
#include <glob.h>
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

static void find_files_recursion(const char* path, setting_t * setting, SEXP_t* files, int depth);
static int init_devs (const char *mtab, char **local, setting_t *setting);
static int match_fs (const char *fs, char **local);
static int recurse_direction(const char *file, char *direction);
static int recurse_filesystem(struct stat *st, dev_t * dev_id_list, int dev_id_count);
static int recurse_follow(struct stat *st, char *follow);


/* **** PUBLIC **** */
SEXP_t * find_files(SEXP_t * spath, SEXP_t *sfilename, SEXP_t *behaviors) {
	char * local_fs[5] = LOCAL_FILESYSTEMS;
	char *name = NULL, *path = NULL;
	int i, rc;
	int max_depth;
	glob_t globbuf;
	setting_t * setting;	
	SEXP_t * files;

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
	rc = glob(path,  0 , NULL, &globbuf);
	if(!rc && globbuf.gl_pathc > 0) {
		for(i=0; i < globbuf.gl_pathc; i++)
			find_files_recursion(globbuf.gl_pathv[i], setting, files, max_depth);
		globfree(&globbuf);
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


/* **** LOCAL **** */

/*
 *  depth defines how many directories to recurse when a recurse direction is specified
 *  '-1' means no limitation
 *  '0' is equivalent to no recursion
 *  '1' means to step only one directory level up/down
 */
static void find_files_recursion(const char* path, setting_t * setting, SEXP_t *files, int depth) {
	struct stat st;
	struct dirent * pDirent;
	DIR * pDir;
	char * path_new;

	pDir = opendir(path);
	if( pDir == NULL) 
		return;

	while ( (pDirent = readdir(pDir)) ) {
		path_new = malloc( (strlen(path)+'/'+strlen(pDirent->d_name)+1)*sizeof(char) );
		sprintf(path_new,"%s/%s",path,pDirent->d_name);

		if( lstat(path_new, &st) == -1)
			continue;

		if( recurse_follow(&st,setting->follow) &&                                        /* follow symlinks? */
		    recurse_direction(pDirent->d_name, setting->direction) &&                /* up or down direction? */
		    depth &&					        	                /* how deep rabbit hole is? */
		    recurse_filesystem(&st, setting->dev_id_list, setting->dev_id_count) ) {                /* local filesystem? */
			find_files_recursion(path_new, setting, files, depth == - 1 ? -1 : depth - 1);
		}
		else if( S_ISREG(st.st_mode) ) {
			/* pattern match on filename*/
			if( regexec(&(setting->re), pDirent->d_name, 0, NULL, 0) == 0 ) {
				SEXP_list_add(files, SEXP_string_newf(path_new));
			}
		}

		free(path_new);
	}

	closedir(pDir);
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

