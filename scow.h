#ifndef SCOW_H
	#define SCOW_H

#define _DEFAULT_SOURCE
#define prints(string)	do { printf(#string " : %s\n", (string)); } while (0)

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>

#include "sds/sds.h"
#include "sds/sdsalloc.h"

typedef enum e_mode {
	COLLECT,
	DEPLOY,
	INVADE,
	TAKEOFF,
} e_mode;

typedef sds t_sds;


t_sds create_dotfiles_dir();
void ask_for_removal(char *file_path);
t_sds sdsdupcatsds(t_sds to_dup,t_sds to_cat);
t_sds sdsdupcat(t_sds to_dup,const char *to_cat);
int setup_collect(const char *item,t_sds dotfiles_path);
void link_rec(const t_sds dir_path,const t_sds dest_path);
e_mode parse_mode(char *mode);
e_mode parse_mode(char *mode);
int record_path(char *,char *);
int record_path(t_sds path_to_record,t_sds location);
int backup_path(char *lol);
int restore_from_backup(char *lol);
int deploy_links_from_paths(char *lol);
int deploy_links_from_paths_backup(char *lol);

#endif
