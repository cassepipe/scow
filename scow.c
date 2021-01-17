/*
 *  TODO :
 *  	- Deploy = invade + backup (.old)
 *
 */

#include "scow.h"

#define _GNU_SOURCE

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
	ret = write(fd, path_to_record, sdslen(path_to_record) + 1);
	close(fd);
	sdsfree(backupfile_path);
	return fd;
}

void ask_for_removal(char *file_path)
{
	int ret;
	int dump;

	fprintf(stderr, "A file with the name %s already exists.\n"
			"Do you want to overwrite it ? (y or n)\n",
			file_path);
	ret = getchar();
	while((dump = getchar()) != '\n' && dump != EOF);
	if (ret == 'y')
	{
		ret = remove(file_path);
	}
	if (ret == -1)
	{
		fprintf(stderr, "Could not overwrite the file. Continue anyways ? (y or n)\n");
		ret = getchar();
		while((dump = getchar()) != '\n' && dump != EOF);
		if (ret != 'y')
			exit(0);
	}
}

void link_rec(const t_sds dir_path, const t_sds dest_path)
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
			link_rec(item_path, new_item_path);
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

int setup_collect(char **items, int number_of_items, t_sds dotfiles_path)
{
	DIR *dir_path_stream;
	char *cwd;
	t_sds item_path;
	t_sds new_link;

	while (number_of_items--)
	{
		//Create an absolute path for the item
		if ((*items)[0] != '/')
		{
			cwd = getcwd(NULL, 0);
			item_path = sdsnew(cwd);
			free(cwd);
			item_path = sdscat(item_path, "/");
			item_path = sdscat(item_path, *items);
		}
		else
			item_path = sdsnew(*items);

		dir_path_stream = opendir(*items);
		if (errno == ENOTDIR)
		{
			new_link = sdsdup(dotfiles_path);
			new_link = sdscat(new_link, *items);
			if (access(new_link, F_OK) == 0)
				ask_for_removal(new_link);
			link(item_path, new_link);
			record_path(item_path, dotfiles_path);
			sdsfree(new_link);
		}
		else if (errno == ENOENT)
		{
			fprintf(stderr, "%s : %s\n", item_path, strerror(errno));
			return (1);
		}
		else
		{
			item_path = sdscat(item_path, "/");
			link_rec(item_path, dotfiles_path);
		}
		sdsfree(item_path);
		closedir(dir_path_stream);
		items++;
	}
	return  (0);
}


int setup_deploy(char **items, int number_of_items, t_sds dotfiles_path)
{
	DIR *dir_path_stream;
	t_sds item_path;
	t_sds scow_file;
	char *filename;
	char item_path_last_char;
	int fd;
	char new_link[BUFFER_SIZE];

	while (number_of_items--)
	{
		if ((*items)[0] != '/')
		{
			//Create an absolute path with dotfiles_path for current item
			item_path = sdsdup(dotfiles_path);
			item_path = sdscat(item_path, *items);
			//Remove trailing '/'
			item_path_last_char = item_path[sdslen(item_path) - 1];
			if (item_path_last_char == '/')
				item_path_last_char = '\0';
			//Get scow_file name
			filename = strrchr(item_path,'/');
			if (!filename)
				filename = item_path;
			scow_file = sdsnew(".");
			scow_file = sdscat(scow_file, filename);
			scow_file = sdscat(scow_file, ".scow");

		}
		else
		{
			fprintf(stderr, "You cannot provide an absolute path in _deploy_ mode.\n"
							"Ignoring %s ...\n",
							item_path);
			sdsfree(item_path);
			items++;
			continue;
		}
		dir_path_stream = opendir(item_path);
		if (errno == ENOTDIR)
		{
			fd = open(scow_file, O_RDONLY);
			read(fd, new_link, BUFFER_SIZE);
			if (access(new_link, F_OK) == 0)
				ask_for_removal(new_link);
			link(item_path, new_link);
			sdsfree(new_link);
		}
		else if (errno == ENOENT)
		{
			fprintf(stderr, "%s : %s\n", item_path, strerror(errno));
			return (1);
		}
		else
		{
			item_path = sdscat(item_path, "/");
			link_rec(item_path, dotfiles_path);
		}
		sdsfree(item_path);
		closedir(dir_path_stream);
		items++;
	}
	return  (0);
}

e_mode parse_mode(char *mode)
{
	char *deploy = "deploy";
	char *invade = "invade";
	char *takeoff = "takeoff";
	char *collect = "collect";
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
	if (!strncmp(mode, collect, len))
		return COLLECT;
	return DEFAULT;
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
	argc--;
	argv++;
	if (argc)
	{
		mode = parse_mode(*argv);
		if (mode)
		{
			argc--;
			argv++;
		}
	}
	if (argc == 0)
	{
		fprintf(stderr, "You must provide a file name\n");
		return (1);
	}

	dotfiles_path = create_dotfiles_dir();

	//dispatch
	switch (mode)
	{
		case DEFAULT:
		case COLLECT:
			setup_collect(argv, argc, dotfiles_path);
			break;
		case DEPLOY:
			setup_deploy(argv, argc, dotfiles_path);
			break;
		case INVADE:
			/*deploy_links_overwrite(argc, argv);*/
			break;
		case TAKEOFF:
			/*restore_from_backup(dotfiles_path);*/
			break;
	}
	sdsfree(dotfiles_path);
}

