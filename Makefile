copytoserv: protocolpro
	cp ./protocol/private_structures.h ./tunserver/private_structures.h
copytoclient: protocolpro2
	cp ./protocol/private_structures.h ./tunclient/private_structures.h
protocolpro:
	gcc -c protocol/protocol.c -o tunserver/protocol.o
protocolpro2:
	gcc -c protocol/protocol.c -o tunclient/protocol.o

