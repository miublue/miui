CC = tcc
CFLAGS = -L/usr/local/lib -lX11 -lraylib -lm

all:
	${CC} -o main main.c ${CFLAGS}

.PHONY: all
