# Compilador
CC=g++

# Flags
CXXFLAGS=-pthread -std=c++11 -lssl -lcrypto
# pthread        -> Threads
# std=c++11      -> Threads c++
# lssl & lcrypto -> SSL

# Executavel
EXEC=testeme

# Arquivos cpp
SRC=$(wildcard src/*.cpp)

#Arquivos Objetos
OBJ=$(SRC:.c=.o)

# Compila
all: $(SRC)
	$(CC) -o $(EXEC) $(OBJ) $(CXXFLAGS)

# Apaga arquivos gerados
# make clean
clean:
	rm $(EXEC) *~ *.o -f
