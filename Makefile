protocol_dir = ./protocol
JSON_LIB = -ljansson
Thred_lib = -lpthread


all: client server

server:	protocol.o server.o
	$(CC) tunserver/main.c protocol.o  server.o -o server $(JSON_LIB) -I $(protocol_dir) $(Thred_lib)
	
client: protocol.o client.o 
	$(CC) tunclient/main.c protocol.o client.o -o client $(JSON_LIB) -I $(protocol_dir) $(Thred_lib)
protocol.o:
	$(CC) -c $(protocol_dir)/protocol.c -o protocol.o -ljansson
server.o:
	$(CC) -c tunserver/server.c -o server.o -I  $(protocol_dir) $(Thred_lib)
client.o:
	$(CC) -c tunclient/client.c -o client.o -I  $(protocol_dir) $(Thred_lib)




clean:
	rm *.o



