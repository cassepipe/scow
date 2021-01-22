/*
 *  TODO :
 *  	- Deploy = invade + backup (.old)
 *
 */

#include "scow.h"
#include <string.h>

/* Under can be removed : Its just there to make the linter happy*/
#define _GNU_SOURCE

#ifndef DT_DIR
	#define DT_DIR 4
#endif

#define prints(string) printf(#string ": %s\n", string)


/* Above can be removed : It's just there to make the linter happy*/

/*==================== UTILS ====================*/

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

void remove_wrapper(char *file_path)
{
	int ret;
	int dump;

	ret = remove(file_path);
	if (ret == -1)
	{
		fprintf(stderr, "Could not overwrite %s. Continue anyways ? (y or n)\n", file_path);
		ret = getchar();
		while((dump = getchar()) != '\n' && dump != EOF);
		if (ret != 'y')
			exit(0);
	}
}

t_sds get_scowfile_name_from_sds( t_sds path)
{
	t_sds scow_file;
	t_sds name;

	name = get_item_name_from_sds(path);
	scow_file = sdsnew(".scow.");
	scow_file = sdscatsds(scow_file, name);
	sdsfree(name);

	return scow_file;
}

t_sds get_item_name_from_sds( t_sds path)
{
	t_sds name;
	char *slash;
	size_t len;

	len = sdslen((t_sds)path);
	if (!len)
		return sdsempty();
	slash = path + len - 1;
	if (slash && *slash ==  '/')
		slash--;
	while (len-- && *slash != '/')
		slash--;
	name = sdsempty();
	if (!len)
		name = sdscatsds(name, path);
	else
		name = sdscat(name, ++slash);
	sdstrim(name, "/");

	return name;
}


/*   _|_|_|    _|_|    _|        _|        _|_|_|_|    _|_|_|  _|_|_|_|_| */
/* _|        _|    _|  _|        _|        _|        _|            _|     */
/* _|        _|    _|  _|        _|        _|_|_|    _|            _|     */
/* _|        _|    _|  _|        _|        _|        _|            _|     */
/*   _|_|_|    _|_|    _|_|_|_|  _|_|_|_|  _|_|_|_|    _|_|_|      _|     */



int setup_collect(char **items, int number_of_items, t_sds dotfiles_path)
{
	DIR *dir_path_stream;
	char *cwd;
	t_sds item_path;
	t_sds new_item_path;
	t_sds scow_file;
	t_sds item_name;

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

		item_name = get_item_name_from_sds(item_path);
		new_item_path = sdsdup(dotfiles_path);
		new_item_path = sdscatsds(new_item_path, item_name);

		dir_path_stream = opendir(item_path);
		if (errno == ENOTDIR)
		{
			if (access(new_item_path, F_OK) == 0)
				ask_for_removal(new_item_path);
			link(item_path, new_item_path);
			scow_file = get_scowfile_name_from_sds(item_name);
			record_path(item_path, scow_file, dotfiles_path);
			sdsfree(scow_file);
		}
		else if (errno == ENOENT)
		{
			fprintf(stderr, "%s : %s\n", item_path, strerror(errno));
			return (1);
		}
		else
		{
			mkdir(new_item_path, 0777);
			item_path = sdscat(item_path, "/");
			new_item_path = sdscat(new_item_path, "/");
			collect_rec(item_path, new_item_path);
		}
		sdsfree(item_path);
		sdsfree(item_name);
		sdsfree(new_item_path);
		closedir(dir_path_stream);
		items++;
	}
	return  (0);

}

//This function assumes dir_path has already been created
void collect_rec(const t_sds dir_path, const t_sds dest_path)
{
	DIR	*dir_path_stream;
	struct dirent *dir_entry;
	t_sds item_path;
	t_sds new_item_path;
	t_sds scow_file;

	record_path(dir_path, ".scow.dir", dest_path);
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
			collect_rec(item_path, new_item_path);
		}
		else
		{
			if (access(new_item_path, F_OK) == 0)
				ask_for_removal(new_item_path);
			link(item_path, new_item_path);
			printf("Linking %s to %s\n", item_path, new_item_path);
			scow_file = get_scowfile_name_from_sds(item_path);
			record_path(item_path, scow_file, dest_path);
			sdsfree(scow_file);
		}
		sdsfree(item_path);
		sdsfree(new_item_path);
	}
	closedir(dir_path_stream);
}

int record_path(const t_sds path_to_record, const char* record_name, const t_sds record_location)
{
	int fd;
	int ret;
	t_sds backupfile_path;

	backupfile_path = sdsempty();
	backupfile_path = sdscat(backupfile_path, record_location);
	backupfile_path = sdscat(backupfile_path, "/");
	backupfile_path = sdscat(backupfile_path, record_name);

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

/*  ____  _____ ____  _     _____   _ _ */
/* |  _ \| ____|  _ \| |   / _ \ \ / /  */
/* | | | |  _| | |_) | |  | | | \ V /   */
/* | |_| | |___|  __/| |__| |_| || |    */
/* |____/|_____|_|   |_____\___/ |_|    */
/*                                      */

int make_backup( t_sds file_path)
{
	return 1;
}

void deploy_rec(const t_sds dir_path, bool backup_flag)
{
	DIR	*dir_path_stream;
	struct dirent *dir_entry;
	t_sds item_path;
	t_sds scow_file;
	char new_dir_path[BUFFER_SIZE];
	char new_item_path[BUFFER_SIZE];
	int fd;
	size_t read_count;

	chdir(dir_path);
	printf("Now in dir %s\n", dir_path);
	fd = open(".scow.dir", O_RDONLY);
	read(fd, new_dir_path, BUFFER_SIZE);
	close(fd);
	mkdir(new_dir_path, 0777);
	printf("Making %s\n", new_dir_path);
	dir_path_stream = opendir(dir_path);
	while ((dir_entry = readdir(dir_path_stream)) != NULL)
	{
		item_path = sdsdup(dir_path);
		prints(dir_entry->d_name);
		item_path = sdscat(item_path, dir_entry->d_name);
		prints(item_path);
		if (dir_entry->d_name[0] == '.'
				&& (dir_entry->d_name[1] == '.' || dir_entry->d_name[1] == '\0'))
			;
		else if (!strncmp(dir_entry->d_name, ".scow.", 6))
			printf("Ignoring scowfile %s\n", dir_entry->d_name);
		else if (dir_entry->d_type == DT_DIR)
		{
			item_path = sdscat(item_path, "/");
			deploy_rec(item_path, backup_flag);
		}
		else
		{
			scow_file = get_scowfile_name_from_sds(item_path);
			prints(scow_file);
			fd = open(scow_file, O_RDONLY);
			read_count = read(fd, new_item_path, BUFFER_SIZE);
			if (access(new_item_path, F_OK) == 0)
			{
				if (backup_flag)
					make_backup(new_item_path);
				remove_wrapper(new_item_path);
			}
			link(item_path, new_item_path);
			sdsfree(scow_file);
		}
		sdsfree(item_path);
	}
	closedir(dir_path_stream);
}


int setup_deploy(char **items, int number_of_items, t_sds dotfiles_path, bool backup_flag)
{
	DIR *dir_path_stream;
	t_sds item_path;
	t_sds scow_file;
	int fd;
	char new_link[BUFFER_SIZE];

	while (number_of_items--)
	{
		if ((*items)[0] != '/')
		{
			prints(*items);
			//Create an absolute path with dotfiles_path for current item
			item_path = sdsdup(dotfiles_path);
			item_path = sdscat(item_path, *items);
			prints(item_path);
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
			//Get scow_file name
			scow_file = get_scowfile_name_from_sds(item_path);
			prints(scow_file);
			//Read the path in hidden .scow file
			fd = open(scow_file, O_RDONLY);
			read(fd, new_link, BUFFER_SIZE);
			printf("Reading path from %s\n", scow_file);
			prints(new_link);
			if (access(new_link, F_OK) == 0)
			{
				if (backup_flag)
					make_backup(new_link);
				remove_wrapper(new_link);
			}
			link(item_path, new_link);
			printf("Linking %s to %s\n", item_path, new_link);
			sdsfree(scow_file);
		}
		else if (errno == ENOENT)
		{
			fprintf(stderr, "%s : %s\n", item_path, strerror(errno));
			return (1);
		}
		else
		{
			item_path = sdscat(item_path, "/");
			prints(item_path);
			deploy_rec(item_path, backup_flag);
		}
		closedir(dir_path_stream);
		sdsfree(item_path);
		items++;
	}
	return  (0);
}

/* _ _  __       _       */
/* |  \/  | __ _(_)_ __  */
/* | |\/| |/ _` | | '_ \ */
/* | |  | | (_| | | | | |*/
/* |_|  |_|\__,_|_|_| |_|*/


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
			setup_deploy(argv, argc, dotfiles_path, true);
			break;
		case INVADE:
			setup_deploy(argv, argc, dotfiles_path, false);
			break;
		case TAKEOFF:
			/*restore_from_backup(dotfiles_path);*/
			break;
	}
	sdsfree(dotfiles_path);
}
