INC_PATH = 
LIB_PATH = 
BIN_PATH = ../bin
LIB = 

CC = g++
CC_FLAG = -g
LINK_FLAG = 

PRG = libprotonet.a
OBJ = buffer.o connector.o fepoll.o fselect.o listener.o network.o platform.o session.o

all:$(PRG)

$(PRG):$(OBJ)
	ar -rc $@ $(OBJ) $(LINK_FLAG) $(LIB_PATH:%=-L%) $(LIB:%=-l%)

.SUFFIXES: .c .cc .o .cpp
.cpp.o:
	$(CC) $(CC_FLAG) $(INC_PATH:%=-I%) -c $*.cpp -o $*.o

.cc.o:
	$(CC) $(CC_FLAG) $(INC_PATH:%=-I%) -c $*.cc -o $*.o

.PRONY:clean
clean:
	rm -f $(OBJ) $(PRG)

install:
	cp -f $(PRG) $(BIN_PATH)

