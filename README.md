Scow
====

__Unfinished__
__Not safe for work__
 __All contributions and advice are more than welcome.__

`scow` aims at quickly gathering and deploying config files.

 The idea is to collect all the config files disseminated in your computer you want to have a quick access to in a `.dotfiles` directory in you home folder.
 Then you should be able to `git clone` your dotfiles repo and have `scow` deploy all the files at the right place so programs can use it. 

 `scow` should allow you to download your config file backup with git, radicle or whatever and have them placed in the right spot, overwriting or making 
 a backup if a file with the same name already exists.

 Ideally in the future `scow` should allow you deploy your config files on a system and give the possibility to restore the config files it replaced.

 This is mostly an exercise to teach myself system calls in C and itch my itch but any new ideas welcome.

 `scow` should stay KISS and work out of the box.

 License is GPLv3 cause Linux saved my old computers from the trash and make the new one shine and because I feel like I owe a lot to the Free Softawre movement.
 
# How it (is supposed to) work
 There are four modes.
 Those are temporary names. Maybe I'll change them for load, unload, dump, sailaway or something related to scows.

## Collect mode
`scow` takes a file and create a hard link in a `~/.dotfiles` directory or takes a directory and reproduces the directory structure recursively with hard links. 
 For each hard link that is created a hidden .scow file is also created alongside to store the path the original file came from.

## Deploy mode
 Deploy your dotfiles at the right spot by reading the hidden `.scow` files and make a backup if file already exist. If there is two or backup files, you should be
 prompted.

##Invade mode
 Same as deploy mode but overwrite config files already present.

##Takeoff mode
 Restore files that were there before deployment.

#Usage
 ```
 scow [mode] file_or_dir  ...
 ```

# Why hard links and not copy ?

 Because it allows to manage all dotfiles from `~/dotfiles` directly.

# Why not use symbolic link ?

 Because `git` don't follow symbolic links (anymore).
 I hear they can't span across filesystems... Well my laptop has only one drive and one main partition.
 Maybe they could be used in deploy mode (non-destructive), could fit the job better maybe.

# Is there a link with GNU stow ?

 I have to admit I wanted a simple program to be able to collect dotfiles in the same place so I can `git push` them in the clouds and then easily deploy
 them on a new machine, eventually non-destructively so I can just then -swoosh- disappear and restore it to how it was. 
 So i stumbled upon GNU stow, but I did not grok it after five minutes of reading so I went with my own thing.
 I am no genius, just trying to get better at programming with system calls in C and to scratch my itch. 

# Dependencies
 The fantastic Antirez's SDS library is extensively used. So familiarize yourself with it before looking at the source code.
 I just added it as a git submodule to this repo.
 [https://github.com/antirez/sds]

TODO
====
* Write `make_backup` for deploy mode
* How to handle if there is already a backup files created by scow ? (Ask the user to choose ?)
* Write takeoff mode
* Check for allocation failures everywhere
* Offer an option to change the .dotfiles location ?
* Write tests ? (How ?) I'd love to use criterion. Run test in container.

