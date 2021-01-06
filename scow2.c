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

void replicate_dir_structure(char *path, char *dest);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
			fprintf(stderr, "You must provide a file name\n");
			return (1);
	}

	char *path = argv[--argc];
	int	path_len = strlen(path);
	if (path[path_len] == '/')
		path[path_len] = '\0';

	char src[256];
	char *sep = strrchr(path, '/');
	strcpy(src, sep + 1);


	char dest[1000];
	strcpy(dest, getenv("HOME"));
	strcat(dest, "/dotfiles/");

	char new_link[1000];
	strcpy(new_link, dest);
	strcat(new_link, src);

//	struct stat sb;

	if (mkdir(dest, 0700) < 0)
	{
		fprintf(stderr, "mkdir() : %s\n", strerror(errno));
	}
	if (link(path, new_link) < 0)
	{
		fprintf(stderr, "link() : %s\n", strerror(errno));
		return 1;
	}

	*sep = '\0';

	return  (0);
}

/*void replicate_dir_structure(char *path, char *dest)*/
/*{*/
	/*char *sep = strrchr(path, '/');*/
	/*for each dir*/
		/*create dir*/
	/*for each directory*/
	/*{*/
		/*replicate_dir_structure(dirname(path));*/
	/*}*/
/*}*/
