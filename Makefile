CC=gcc 
CXX=g++ 

SRC=./*.c 
BIN=./dailyverse 
LIBS=-DIN_LIBXML -lxml2 -lz -liconv -llzma -lws2_32 -lcurl 

LIBDIR=C:\msys64\mingw64\lib
INCLUDE=C:\msys64\mingw64\include
XML_INCLUDE=C:\msys64\mingw64\include\libxml2 

default: 
	$(CC) $(SRC) -o $(BIN) -I $(INCLUDE) -I $(XML_INCLUDE) -L $(LIBDIR) $(LIBS) 

run: default 
	$(BIN)

	