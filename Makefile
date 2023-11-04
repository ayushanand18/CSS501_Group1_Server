build:
	g++ -Wall src/server.cpp -o src/server.out -I/usr/local/include/rpc -L/usr/local/lib -lrpc -lpthread -lcrypto -lssl -lz

test:
	./src/server.out

clean:
	$(RM) src/server.out