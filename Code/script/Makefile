CC = g++
LFLAGS = -o
CFLAGS = -c -g -Wall -Werror
INC = ./inc
IFLAG = -I$(INC)
BIN = ./bin
OBJ = ./obj
SRC = ./src
SVFLAGS = -v --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valserver 
CVFLAGS = -v --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valclient


all: 	client server valgrindserver valgrindclient 

client: $(OBJ)/client.o
	$(CC) $(LFLAGS) $(BIN)/client $(OBJ)/client.o

$(OBJ)/client.o: $(SRC)/client.cpp
	$(CC) $(CFLAGS) $(IFLAG) $(SRC)/client.cpp 
	mv *.o $(OBJ)

server: $(OBJ)/server.o
	$(CC) $(LFLAGS) $(BIN)/server $(OBJ)/server.o

$(OBJ)/server.o: $(SRC)/server.cpp
	$(CC) $(CFLAGS) $(IFLAG) $(SRC)/server.cpp 
	mv *.o $(OBJ)


clean:
	rm -f *.o a.out
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/server $(BIN)/client
	
	@echo "Cleaning Done!"

valgrindserver:
	valgrind $(SVFLAGS) $(BIN)/server 
	mv valserver $(BIN)/

valgrindclient:
	valgrind $(CVFLAGS) $(BIN)/client 
	mv valclient $(BIN)/
