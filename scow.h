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
#include <ctype.h>
#include <stdbool.h>

#include "sds/sds.h"
#include "sds/sdsalloc.h"


#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#ifndef BUFFER_SIZE
	#define BUFFER_SIZE 2048
#endif

typedef sds t_sds;

t_sds create_dotfiles_dir();

enum e_mode {
	DEFAULT = 0,	
	COLLECT,
	DEPLOY,
	INVADE,
	TAKEOFF,
};

typedef enum e_mode e_mode;

/*FUNCTIONS*/
t_sds sdsdupcat(t_sds to_dup,const char *to_cat);
t_sds sdsdupcatsds(t_sds to_dup,t_sds to_cat);
void ask_for_removal(char *file_path);
void remove_wrapper(char *file_path);
int setup_collect(char **items,int number_of_items,t_sds dotfiles_path);
void collect_rec(const t_sds dir_path,const t_sds dest_path);
int record_path(const t_sds path_to_record,const char *record_name,const t_sds record_location);
t_sds give_scowfile_name(const char *path);
t_sds get_item_name(const char *path);
int make_backup(t_sds file_path);
void deploy_rec(const t_sds dir_path,bool backup_flag);
int setup_deploy(char **items,int number_of_items,t_sds dotfiles_path,bool backup_flag);
e_mode parse_mode(char *mode);
t_sds create_dotfiles_dir();
