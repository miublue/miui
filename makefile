CC = tcc
LIBS = -lX11 -lraylib -lm
CFLAGS = -Wall -Werror -Wextra -I/usr/local/include -L/usr/local/lib

all:
	${CC} -o main main.c ${LIBS} ${CFLAGS}

.PHONY: all
