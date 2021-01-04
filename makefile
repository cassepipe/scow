SRC		=	scow.c	

CC		=	gcc

CFLAGS	=	-Wall -Wextra -g3

LDFLAGS	=	-fsanitize=address


scow	: 	scow.o

clean	:
			rm -rf scow.o
			rm -rf dotfiles
			rm -rf ~/dotfiles

fclean	:	clean
			rm -rf scow

re		:	fclean scow

.PHONY	:	clean fclean re
