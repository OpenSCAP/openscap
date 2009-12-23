#pragma once
#ifndef FSDEV_H
#define FSDEV_H

#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>

typedef struct {
	dev_t *ids;
	uint16_t cnt;
} fsdev_t;

fsdev_t *fsdev_init(const char **fs, size_t fs_cnt);
fsdev_t *fsdev_strinit(const char *fs_names);
void fsdev_free(fsdev_t * lfs);

int fsdev_search(fsdev_t * lfs, void *id);
int fsdev_path(fsdev_t * lfs, const char *path);
int fsdev_fd(fsdev_t * lfs, int fd);

#endif				/* FSDEV_H */
