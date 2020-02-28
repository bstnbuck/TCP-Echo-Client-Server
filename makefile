CC=cc
LINK=cc
CFLAGS=-g -Wall -Wextra
LFLAGS= -o $@
LIBDIR=
LIBS=
OBJ=o
del=rm
EXE=

all : server client

% : %.$(OBJ)
	$(LINK) $(LFLAGS) $@.$(OBJ) $(LIBS)

%.$(OBJ) : %.c
	$(CC) -c $@.c $(CFLAGS)

clean : ;
	-$(del)  server$(EXE) client$(EXE) *.$(OBJ) 
