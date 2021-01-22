# **************************************************************************** #
#                
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tpouget <cassepipe@ymail.com>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/23 16:37:33 by tpouget           #+#    #+#              #
#    Updated: 2021/01/15 15:45:54 by tpouget          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#	Variables

PHONY			=	all clean fclean re update see ctags

SOURCEFILES		=	scow.c \
					sds/sds.c

OBJECTFILES		=	$(patsubst %.c,obj/%.o,$(SOURCEFILES))
	
HEADERFILES		=	$(patsubst %.c,%.h,$(SOURCEFILES)) \
	
CFLAGS			=	-Wall -Wextra 


ifeq ($(DEBUG), debugger)
	CFLAGS 			+=	 -g3
endif
ifeq ($(DEBUG), sanitizer)
	CFLAGS 			+=	 -g3
	LDFLAGS			=	-fsanitize=address
endif

CC	  		  	=	clang

NAME			=	a.out


#	Rules

all:			${NAME} 

${NAME}:		${OBJECTFILES} ${HEADERFILES} update
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
see:			
				ls -a -R --color  ~/.dotfiles
ctags:
				ctags ${SOURCEFILES}

update:			ctags
				sed -i '/FUNCTIONS/q' scow.h 
				~/makeheaders/makeheaders -h scow.c | head -n -8 >> scow.h
copy:	
				cp -R ../.vim /tmp/.vim

tmp_clean :		
				rm -rf /tmp/.vim

.PHONY:			${PHONY}	
