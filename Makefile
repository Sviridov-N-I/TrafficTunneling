protocol_dir = ./protocol
JSON_LIB = -ljansson
Thred_lib = -lpthread


release: client server
debug: client-dbg server-dbg

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


server-dbg:	protocol.o server-dbg.o
	$(CC) tunserver/main.c protocol.o  server-dbg.o -o server $(JSON_LIB) -I $(protocol_dir) $(Thred_lib) -DDEBUG
	
client-dbg: protocol.o client-dbg.o 
	$(CC) tunclient/main.c protocol.o client-dbg.o -o client $(JSON_LIB) -I $(protocol_dir) $(Thred_lib) -DDEBUG
server-dbg.o:
	$(CC) -c tunserver/server.c -o server-dbg.o -I  $(protocol_dir) $(Thred_lib) -DDEBUG
client-dbg.o:
	$(CC) -c tunclient/client.c -o client-dbg.o -I  $(protocol_dir) $(Thred_lib) -DDEBUG



clean:
	rm *.o



