all:webserver
	touch webserver.c
webserver:socket.h socket.c webserver.c
	gcc socket.c webserver.c -o webserver
clean:
	rm -f *.o *.exe *.out
	rm webserver
