CC ?= gcc

COBJS ?= client.o lib_client.o bmp.o couleur.o jsonParser.o validateur.o
SOBJS ?= serveur.o lib_server.o jsonParser.o validateur.o
TOBJS ?= auto_test.o bmp.o couleur.o jsonParser.o validateur.o lib_server.o lib_client.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client
TEST = auto_test

all: $(SERVER) $(CLIENT) $(TEST)

$(SERVER): $(SOBJS)
	$(CC) -o $(SERVER) $(SOBJS) -lm

$(CLIENT): $(COBJS)
	$(CC) -o $(CLIENT) $(COBJS)

$(TEST): $(TOBJS)
	$(CC) -o $(TEST) $(TOBJS) -lm

.c.o:
	$(CC) -c $*.c
