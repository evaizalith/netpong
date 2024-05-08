server: 
	clang server.c network.c bounce2d.c paddle.c -lcurses -o server

client:
	clang client.c network.c bounce2d.c paddle.c -lcurses -o client

all: clean server client

clean:
	rm -f client server bounce
