#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>


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

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
			fprintf(stderr, "You must provide a file name\n");
			return (1);
	}

	char *src = argv[--argc];

	char dest[1000];
	strcpy(dest, getenv("HOME"));
	strcat(dest, "/dotfiles/");

	char new_link[1000];
	strcpy(new_link, dest);
	strcat(new_link, src);

	struct stat sb;

	if (mkdir(dest, 0700) < 0)
	{
		fprintf(stderr, "mkdir() : %s\n", strerror(errno));
	}
	if (link(src, new_link) < 0)
	{
		fprintf(stderr, "link() : %s\n", strerror(errno));
		return 1;
	}
	return  (0);
}

