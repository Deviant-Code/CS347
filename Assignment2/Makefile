CC=gcc
CFLAGS= -Wall -w -g
INCLUDE:= -Iinclude/
SOURCE:= src/Cli.c
EXE=Cli

SOURCE2:= src/Listf.c
EXE2=src/listf

SOURCE3:= src/Calc.c
EXE3=src/calc

all: $(EXE2)

$(EXE2): $(SOURCE2)
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE2) -o $(EXE2)

all: $(EXE3)

$(EXE3): $(SOURCE3)
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE3) -o $(EXE3)

all: $(EXE)

$(EXE): $(SOURCE)
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) -o $(EXE)
