
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>


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
** Scow aims to follow the KISS philosophy.
**
** Scow is licensed under GPL3 .
*/

in_in_set();
link_and_record_path_rec();
deploy_links_from_paths_backup();
deploy_links_from_paths();
restore_from_backup();
is_dir();

int link_and_record_path_rec(int argc, char *path)
{
	int	path_len = strlen(path);
	if (path[path_len] == '/')
		path[path_len] = '\0';

	char target[256];
	char *sep = strrchr(path, '/');
	strcpy(target, sep + 1);
	if (is_dir(target))
	{
		replicate_dir_structure(path);
	}
	else
	{

		char dest[1000];
		strcpy(dest, getenv("HOME"));
		strcat(dest, "/dotfiles/");

		char new_link[1000];
		strcpy(new_link, dest);
		strcat(new_link, target);

		mkdir(dest, 0700)

		if (link(path, new_link) < 0)
		{
			fprintf(stderr, "link() : %s\n", strerror(errno));
			return 1;
		}
	}


	return  (0);
}

void replicate_dir_structure(char *path, char *dest)
{
	char *sep = strrchr(path, '/');
	for each dir
		create dir
	for each directory
	{
		replicate_dir_structure(dirname(path));
	}
}


typedef enum e_mode {
	ERROR,
	COLLECT,
	DEPLOY,
	INNVADE,
	TAKEOFF,
} e_mode;

e_mode parse_args(int argc, char *mode)
{
	char *deploy = "deploy";
	char *invade = "invade";
	char *takeoff = "takeoff";
	int i;
	int len;

	i = 0;
	len = strlen(mode);
	if (!strncmp(mode, deploy, len)
			return DEPLOY;
	if (!strncmp(mode, invade, len)
			return INVADE;
	if (!strncmp(mode, deploy, len)
			return TAKEOFF;
	return COLLECT;
}

int  main()
{
	e_mode mode;

	mode = COLLECT;
	if (argc < 2)
	{
			fprintf(stderr, "You must provide a file name\n");
			return (ERROR);
	}
	if (argc > 2)
		mode = parse_mode(argc, argv[2]);


	--argc;
	switch (mode)
	{
		case ERROR:
			return 1;
		case COLLECT:
			link_and_record_path_rec(argv[argc]);
		case DEPLOY:
			deploy_links_from_paths_backup(argv[argc]);
		case INVADE:
			deploy_links_from_paths(argv[argc]);
		case TAKEOFF:
			restore_from_backup(argv[argc]);
	}
}



