CC=     g++

O = ./.obj/

LFLAGS= -D_THREAD_SAFE -lpthread 

CFLAGS= -DVERSION=\"`cat .version`R\" -DOS="\"`uname -srm`\"" -g

HEADERS = Socket.h PriorityQueue.h SocketStream.h ServerSocket.h ClientSocket.h Vector.h Thread.h Directory.h Connection.h vars.h typedef.h fcns.h BST.h QFileDatabase.h RegularExpression.h 

PROGS = version strip disp m2exec

CFILES = Socket.c SocketStream.c ServerSocket.c ClientSocket.c main.c utl.c io.c misccmd.c message.c commands.c menus.c log.c mail.c

OFILES = $(O)main.o $(O)Socket.o $(O)SocketStream.o $(O)ServerSocket.o $(O)ClientSocket.o $(O)utl.o $(O)io.o $(O)misccmd.o $(O)message.o $(O)menus.o $(O)commands.o $(O)log.o $(O)mail.o

all: $(PROGS)

disp: disp.c
	$(CC) disp.c -o disp

strip:  strip.c
	$(CC) strip.c -o strip

version: version.c 
	g++ version.c -o version

.version: 
	./version >.version

min:
	./version min

max:	
	./version max

fcns:
	rm fcns.h
	echo $(CFILES) > tmpfile.txt
	./strip fcns.h tmpfile.txt fcns.all
	rm tmpfile.txt
	
m2exec: .version .obj $(OFILES) Makefile 
	$(CC) $(LFLAGS) -o m2exec $(OFILES)
	mv m2exec ..
	rm .version

$(O)Socket.o: Socket.h Socket.c Makefile
	$(CC) $(CFLAGS) -c Socket.c -o $(O)Socket.o
	
$(O)SocketStream.o: Socket.h Socket.c SocketStream.c SocketStream.h Makefile
	$(CC) $(CFLAGS) -c SocketStream.c -o $(O)SocketStream.o
	
$(O)ServerSocket.o: ServerSocket.h Socket.h Socket.c ServerSocket.c Makefile
	$(CC) $(CFLAGS) -c ServerSocket.c -o $(O)ServerSocket.o
	
$(O)ClientSocket.o: ClientSocket.h ClientSocket.c Socket.h Socket.c Makefile
	$(CC) $(CFLAGS) -c ClientSocket.c -o $(O)ClientSocket.o
	
$(O)main.o: $(HEADERS) main.c Makefile
	$(CC) $(CFLAGS) -c main.c -o $(O)main.o

$(O)utl.o: $(HEADERS) utl.c Makefile
	$(CC) $(CFLAGS) -c utl.c -o $(O)utl.o

$(O)io.o: $(HEADERS) io.c Makefile
	$(CC) $(CFLAGS) -c io.c -o $(O)io.o

$(O)misccmd.o: $(HEADERS) misccmd.c Makefile
	$(CC) $(CFLAGS) -c misccmd.c -o $(O)misccmd.o

$(O)message.o: $(HEADERS) message.c Makefile
	$(CC) $(CFLAGS) -c message.c -o $(O)message.o

$(O)commands.o: $(HEADERS) commands.c Makefile
	$(CC) $(CFLAGS) -c commands.c -o $(O)commands.o

$(O)menus.o: $(HEADERS) menus.c Makefile
	$(CC) $(CFLAGS) -c menus.c -o $(O)menus.o

$(O)log.o: $(HEADERS) log.c Makefile
	$(CC) $(CFLAGS) -c log.c -o $(O)log.o

$(O)mail.o: $(HEADERS) mail.c Makefile
	$(CC) $(CFLAGS) -c mail.c -o $(O)mail.o

clean:
	if [ -f disp ]; then rm disp; fi
	if [ -f strip ]; then rm strip; fi
	if [ -f version ]; then rm version; fi
	rm ./.obj/* *~
.obj:
	mkdir .obj
