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

typedef enum e_mode {
	COLLECT,
	DEPLOY,
	TAKEOFF,
	ERROR,
} e_mode;

e_mode check_args(int argc, char **argv)
{
	if (argc < 2 )
	{
			fprintf(stderr, "You must provide a file name\n");
			return (ERROR);
	}
	if (strcmp(argv[1], "deploy") <

	return (COLLECT);
}

int main(int argc, char *argv[])
{
	e_mode mode;

	mode = check_args(argc, argv);

	char *path = argv[--argc];

	char dest[1000];
	strcpy(dest, getenv("HOME"));
	strcat(dest, "/dotfiles/");

	mkdir(dest, 0700);
	execl("/bin/cp", "-l", "-r", path, dest, (char*)0);

	return  (0);
}
