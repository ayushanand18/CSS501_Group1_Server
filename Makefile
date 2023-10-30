all:
	g++ src/server.cpp -o src/server.out -I/usr/local/include/rpc -L/usr/local/lib -lrpc -lpthread

test:
	echo "Running Tests..."

clean:
	$(RM) src/server.out