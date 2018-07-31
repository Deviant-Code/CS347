CC=gcc
CFLAGS= -Wall -w -g
INCLUDE:= -Iinclude/
SOURCE:= source/quickfit.c \
		 source/HeapTestEngine.o
EXE=QuickFit

all: $(EXE)

$(EXE): $(SOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) -o $(EXE)
