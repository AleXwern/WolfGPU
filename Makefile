# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/01/07 12:41:01 by anystrom          #+#    #+#              #
#    Updated: 2021/05/01 01:01:16 by anystrom         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = wolf3d
FLG =
SRCFILE = wolf.c fileformat.c gfx.c key_input.c render.c draw.c move.c randenc.c \
			floor.c randkey.c interact.c util.c anim.c ai.c entity.c cursor.c \
			bmp_reader.c
SRCFILE = wolf.c fileformat.c gpu.c render.c bmp_reader.c gfx.c
SRC = $(addprefix ./src/,$(SRCFILE))
LIBFT = ./obj/libft.a
OBJS = $(SRC:.c=.o)
OBJ = $(addprefix ./obj/,$(SRCFILE:.c=.o))
OBJDIR = ./obj/
SRCDIR = ./src/
INCL = -I ./libft \
	-I ./includes \
	-I C:\SDL\SDL2-2.0.12\include \
	-I C:\Users\alexn\Desktop\Rando-code\glew-2.1.0\include\GL \
	-I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\include"
LIB = C:/SDL/SDL2-2.0.12/lib/x64/SDL2main.lib \
	C:/SDL/SDL2-2.0.12/lib/x64/SDL2.lib \
	"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\lib\x64\OpenCL.lib"
RED = \033[0;31m
STOP = \033[0m

.PHONY: all clean fclean re obj test run

all: $(NAME)

$(LIBFT):
	@echo Compiling Libft libraries.
	@make -C ./libft

$(OBJDIR)%.o:$(SRCDIR)%.c
	@echo "Compiling Wolf3D -> $(RED)$@$(STOP)"
	@gcc $(FLG) $(INCL) -o $@ -c $<

$(NAME): $(OBJ) $(LIBFT)
	gcc $(FLG) $(INCL) -o $(NAME) $(OBJ) $(LIBFT) $(LIB)
	@echo Done.

test:
	gcc $(FLG) $(INCL) -o test.exe src/test.c $(LIBFT) $(LIB)
	./test.exe
	rm ./test.exe

run: $(NAME)
	./wolf3d.exe

clean:
	@echo "Removing Wolf3D libraries."
	@/bin/rm -f $(OBJ)
	@echo Removing Libft libraries.
	@make -C ./libft fclean
	@/bin/rm -f test.exe

fclean: clean
	@echo Removing binaries.
	@/bin/rm -f $(NAME)

re: fclean all
