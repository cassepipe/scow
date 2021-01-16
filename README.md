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
