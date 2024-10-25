IDIR = include
CC = g++
CFLAGS = -I. -I$(IDIR)
DEPS = $(IDIR)/Client.h $(IDIR)/Server.h $(IDIR)/Notify.h
OBJSERVER = src/server/main.o src/server/Server.o 
OBJCLIENT = src/client/main.o src/client/Client.o src/client/Notify.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

default: server client

server: $(OBJSERVER)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(OBJCLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf server client src/server/main.o src/server/Server.o src/client/main.o src/client/Client.o src/client/Notify.o
