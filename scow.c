/*
** `scow` aims at quickly managing and deploying config files.
** `scow` takes a file and create a hard link in `~/.dotfiles` or takes a directory and reproduces the directory structure recursively
** with hard links. For each hard link that is created a hidden file is created to store where the original file came from.
**
** Why hard links and not copy ? Because it allows to manage all dotfiles from `~/dotfiles` directly.
**
**
** `scow` should allow you to download your config files with git, radicle or even an email provider and have them placed in the right spot,
** replacing and making a backup of existing config files if already exist.
** Ideally in the future `scow` should allow you deploy your config files on a system and give the possibility to restore the config files it replaced.
**
** The program's argument are a mode (collect is default and can be omitted) and then a path. If the path is file, only the file will be linked
** to the `~/.dotfiles` directory. It the path is of a directory, files links and subdirectories will be created recursively.
**
** Scow aims to follow the KISS philosophy.
**
** Scow is licensed under GPL3 .
*/

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "sds.h"

typedef enum e_mode {
	COLLECT,
	DEPLOY,
	INVADE,
	TAKEOFF,
} e_mode;

typedef sds t_sds;

int in_in_set();
int link_and_record_path_rec();
int deploy_links_from_paths_backup();
int deploy_links_from_paths();
int restore_from_backup();
int is_dir();
int backup_path();
e_mode parse_mode();

void replicate_dir_structure();

int link_and_record_path_rec(char *path, char *dotfiles_path)
{
	t_sds new_link;
	char *file_name;
	DIR *dir_path_stream;

	path = sdsnew(path);
	dotfiles_path = sdsnew(dotfiles_path);

	dir_path_stream = opendir(path);
	if (errno == ENOTDIR)
	{
		if (path[sdslen(path) - 1] == '/')
			path[sdslen(path) - 1] = 0;
		file_name = strrchr(path, '/');
		new_link = sdsnew(dotfiles_path);
		new_link = sdscat(new_link, ++file_name);

		if (link(path, new_link) < 0)
		{
			fprintf(stderr, "link() : %s\n", strerror(errno));
			return 1;
		}
		sdsfree(new_link);
	}
	else if (errno == ENOENT)
	{
		perror("Error :");
	}
	else
	{
		replicate_dir_structure(path, dotfiles_path);
	}
	return  (0);
}

void replicate_dir_structure(const t_sds dir_path, const t_sds dest_path)
{
	DIR	*dir_path_stream;
	struct dirent *dir_entry;
	t_sds item_path;
	t_sds new_item_path;

	dir_path_stream = opendir(dir_path);
	while ((dir_entry = readdir(dir_path_stream)) != NULL)
	{
		item_path = sdsdup(dir_path);
		new_item_path = sdsdup(dest_path);
		item_path = sdscat(item_path, dir_entry->d_name);
		new_item_path = sdscat(new_item_path, dir_entry->d_name);
		if (dir_entry->d_type == DT_DIR)
		{
			mkdir(new_item_path, 0777);
			replicate_dir_structure(item_path, new_item_path);
		}
		else
		{
			link(item_path, new_item_path);
		}
		sdsfree(item_path);
		sdsfree(new_item_path);
	}
	closedir(dir_path_stream);
}

e_mode parse_args(int argc, char *mode)
{
	char *deploy = "deploy";
	char *invade = "invade";
	char *takeoff = "takeoff";
	int i;
	int len;

	i = 0;
	len = strlen(mode);
	if (!strncmp(mode, deploy, len))
			return DEPLOY;
	if (!strncmp(mode, invade, len))
			return INVADE;
	if (!strncmp(mode, takeoff, len))
			return TAKEOFF;
	return COLLECT;
}

int  main(int argc, char *argv[])
{
	e_mode	mode;
	t_sds	dotfiles_path;

	//parsing args
	mode = COLLECT;
	if (argc < 2)
	{
			fprintf(stderr, "You must provide a file name\n");
			return (1);
	}
	if (argc > 2)
		mode = parse_mode(argc, argv[2]);

	//get dotfiles directory path
	dotfiles_path = sdsnew(getenv("HOME"));
	dotfiles_path = sdscat(dotfiles_path, "/.dotfiles/");
	mkdir(dotfiles_path, 0777);

	//dispatch
	--argc;
	switch (mode)
	{
		case COLLECT:
			link_and_record_path_rec(argv[argc], dotfiles_path);
			break;
		case DEPLOY:
			deploy_links_from_paths_backup(argv[argc]);
			break;
		case INVADE:
			deploy_links_from_paths(argv[argc]);
			break;
		case TAKEOFF:
			restore_from_backup(argv[argc]);
			break;
	}
	sdsfree(dotfiles_path);
}



