IDIR = include
CC=g++
CFLAGS=-I. -I$(IDIR)
DEPS = $(IDIR)/Client.h $(IDIR)/Server.h
OBJSERVER = src/server/main.o src/server/Server.o 
OBJCLIENT = src/client/main.o src/client/Client.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJSERVER)
	$(CC) -o $@ $^ $(CFLAGS)


client: $(OBJCLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf server client src/server/main.o src/server/Server.o  src/client/main.o src/client/Client.o