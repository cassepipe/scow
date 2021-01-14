# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tpouget <cassepipe@ymail.com>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/23 16:37:33 by tpouget           #+#    #+#              #
#    Updated: 2021/01/14 15:31:17 by tpouget          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#	Variables

PHONY			=	all clean fclean re

SOURCEFILES		=	scow.c \
					sds/sds.c

OBJECTFILES		=	$(patsubst %.c,obj/%.o,$(SOURCEFILES))
	
HEADERS			=	sds/sds.h \
					sds/sdsalloc.h
	
CFLAGS			=	-Wall -Wextra 


ifeq ($(DEBUG), 1)
	CFLAGS 			+=	 -g3
endif
ifeq ($(DEBUG), 2)
	CFLAGS 			+=	 -g3
	LDFLAGS			=	-fsanitize=address
endif

CC	  		  	= 	clang

NAME			=	a.out


#	Rules

all:			${NAME}

${NAME}:		${OBJECTFILES} ${HEADERS}
				${CC} ${LDFLAGS} ${OBJECTFILES}

obj/%.o:		%.c | obj obj/sds
				${CC} ${CFLAGS} -c $< -o $@
obj:
				mkdir obj

obj/sds			: | obj
				mkdir obj/sds
clean:			
				rm -rf obj

fclean:			clean
				rm -rf ${NAME}

re:				fclean dotfiles_clean all 

dotfiles_clean:
				rm -rf ~/.dotfiles

run:			dotfiles_clean	
				./${NAME} lol
see:			
				ls -a ~/.dotfiles
				ls -a ~/.dotfiles/hola

.PHONY:			${PHONY}	
