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

int in_in_set()
{
	return 1;
}
int link_and_record_path_rec();
int deploy_links_from_paths_backup(char* lol)
{
	return 1;
}
int deploy_links_from_paths(char* lol)
{
	return 1;
}
int restore_from_backup(char* lol)
{
	return 1;
}
int backup_path(char* lol)
{
	return 1;
}

int record_path(char *, char*);

e_mode parse_mode(char *mode);

void replicate_dir_structure();

int record_path( t_sds path_to_record, t_sds location)
{
	int fd;
	int ret;
	t_sds filename;
	char *sep;
	char *backupfile_path;

	sep = strrchr(path_to_record, '/');
	filename = sdsnew(".");
	filename = sdscat(filename, ++sep);
	filename = sdscat(filename, ".scow");
	backupfile_path = sdsdup(location);
	backupfile_path = sdscat(backupfile_path, "/");
	backupfile_path = sdscatsds(backupfile_path, filename);
	sdsfree(filename);

	fd = creat(backupfile_path, S_IRWXU);
	if (fd < 0)
	{
		fprintf(stderr, "%s : %s\n", backupfile_path, strerror(errno));
	}
	ret = write(fd, path_to_record, sdslen(path_to_record));
	close(fd);
	sdsfree(backupfile_path);
	return fd;
}


int setup_collect(const char *item, t_sds dotfiles_path)
{
	DIR *dir_path_stream;
	char *cwd;
	t_sds item_path;
	t_sds new_link;

	//Create an absolute path for the item
	if (item[0] != '/')
	{
		cwd = getcwd(NULL, 0);
		item_path = sdsnew(cwd);
		free(cwd);
		item_path = sdscat(item_path, "/");
		item_path = sdscat(item_path, item);
	}
	else
		item_path = sdsnew(item);

	dir_path_stream = opendir(item);
	if (errno == ENOTDIR)
	{
		new_link = sdsdup(dotfiles_path);
		new_link = sdscat(new_link, item);
		link(item_path, new_link);
		sdsfree(new_link);
	}
	else if (errno == ENOENT)
	{
		perror("Error ");
		return (1);
	}
	else
	{
		item_path = sdscat(item_path, "/");
		replicate_dir_structure(item_path, dotfiles_path);
	}
	sdsfree(item_path);
	closedir(dir_path_stream);
	return  (0);
}

t_sds sdsdupcat( t_sds to_dup, const char* to_cat)
{
	t_sds ret;

	ret = sdsdup(to_dup);
	ret = sdscat(ret, to_cat);

	return ret;
}

t_sds sdsdupcatsds( t_sds to_dup, t_sds to_cat)
{
	t_sds ret;

	ret = sdsdup(to_dup);
	ret = sdscatsds(ret, to_cat);

	return ret;
}

void ask_for_removal(char *file_path)
{
	int ret;

	fprintf(stderr, "A file with the name %s already exists.\n"
			"Do you want to overwrite it ? (y or n)",
			file_path);
	while (!(ret = getchar()))
		;
	if (ret == 'y')
		ret = remove(file_path);
	if (ret == -1)
	{
		fprintf(stderr, "Could not overwrite the file. Continue anyways ? (y or n)");
		while (!(ret = getchar()))
			;
	}
	if (ret == 'y')
		return;
	else
		exit(0);
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
		if (dir_entry->d_name[0] == '.'
				&& (dir_entry->d_name[1] == '.' || dir_entry->d_name[1] == '\0'))
					;
		else if (dir_entry->d_type == DT_DIR)
		{
			mkdir(new_item_path, 0777);
			item_path = sdscat(item_path, "/");
			new_item_path = sdscat(new_item_path, "/");
			//printf("replicate_dir_structure(%s, %s)\n", item_path, new_item_path);
			replicate_dir_structure(item_path, new_item_path);
		}
		else
		{
			//printf("link(%s, %s)\n", item_path, new_item_path);
			if (access(new_item_path, F_OK) == 0)
				ask_for_removal(new_item_path);
			link(item_path, new_item_path);
			record_path(item_path, dest_path);
		}
		sdsfree(item_path);
		sdsfree(new_item_path);
	}
	closedir(dir_path_stream);
}

e_mode parse_mode(char *mode)
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

t_sds create_dotfiles_dir()
{
	t_sds dotfiles_path;

	dotfiles_path = sdsnew(getenv("HOME"));
	dotfiles_path = sdscat(dotfiles_path, "/.dotfiles/");
	mkdir(dotfiles_path, 0777);

	return dotfiles_path;
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
		mode = parse_mode(argv[2]);

	dotfiles_path = create_dotfiles_dir();

	//dispatch
	--argc;
	switch (mode)
	{
		case COLLECT:
			setup_collect(argv[argc], dotfiles_path);
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



