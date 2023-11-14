build:
	g++ -std=c++20 -Wall src/server.cpp -o src/server.out -I/usr/local/include/rpc -L/usr/local/lib -lrpc -lpthread -lcrypto -lssl -lz -lgcov

run:
	./src/server.out

clean:
	$(RM) src/server.out