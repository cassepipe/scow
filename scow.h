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

#include "sds/sds.h"
#include "sds/sdsalloc.h"

#define _GNU_SOURCE

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
int record_path(t_sds path_to_record,t_sds location);
void ask_for_removal(char *file_path);
void link_rec(const t_sds dir_path,const t_sds dest_path);
int setup_collect(char **items,int number_of_items,t_sds dotfiles_path);
e_mode parse_mode(char *mode);
t_sds create_dotfiles_dir();
int setup_deploy(char **items,int number_of_items,t_sds dotfiles_path);
