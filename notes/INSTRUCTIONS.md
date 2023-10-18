# Instructions
**Duplicated from Client Repo for ease**
> These include instructions to setup the client or the server code using RPCLib.

## Installation
> If you need to install the RPCLib library from the source fresh. You must skip if you are not building from source. If you are cloning this repo, simply skip over to Next Steps.
On Linux:
+ `git clone https://github.com/rpclib/rpclib`
+ `cd rpclib/`
+ `cmake -DCMAKE_INSTALL_PREFIX=rpc_lib`
+ `make install`
+ Move the `rpc_lib/` directory from inside `rpclib` to outside of it.
+ Delete the `rpclib/` directory by `rm -rf rpclib/`

## Next Steps (Installation)
+ `cd rpc_lib/`
+ `sudo cp -rf include/rpc /usr/local/include/`
+ `sudo cp lib/librpc.a /usr/local/lib/`

## Running
> Execute these steps once you have installed using the above steps. You need to install only once!
Now the library is fully installed on your machine. You will need to link it with the compiler during compilation (you will need to do each time you are compiling `main.cpp`).
* `g++ src/client.cpp -o client.out -I/usr/local/include/rpc -L/usr/local/lib -lrpc -lpthread`
* Run the executable now: `./main`