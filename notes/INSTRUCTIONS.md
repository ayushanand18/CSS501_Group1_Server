# Instructions
> These include instructions to setup the Server.

## Installation
### Pre-requisite
> If you need to install the RPCLib library from the source fresh. You must skip if you are not building from source. If you are cloning this repo, simply skip over to Next Steps.
On Linux:
+ `git clone https://github.com/qchateau/rpclib`
+ `cd rpclib/`
+ `cmake -DCMAKE_INSTALL_PREFIX=rpc_lib`
+ `make install`
+ Move the `rpc_lib/` directory from inside `rpclib` to outside of it.
+ Delete the `rpclib/` directory by `rm -rf rpclib/`

### Next Steps
> Continue from here if you simply cloned this repo.
+ `cd rpc_lib/`
+ `sudo cp -rf include/rpc /usr/local/include/`
+ `sudo cp lib/librpc.a /usr/local/lib/`

## Running
> Execute these steps once you have installed using the above steps. You need to install only once!
Now the library is fully installed on your machine. You will need to link it with the compiler during compilation (you will need to do each time you are compiling `main.cpp`).
* `g++ src/server.cpp -o src/server.out -I/usr/local/include/rpclib -L/usr/local/lib -lrpclib -lpthread`
* Run the executable now: `./src/server.out`

# Run full package
> To run the full suite you must have the server code also on your local machine alongside the client machine.
## First-time run
> Execute these if you are running for the first time
```shell
git clone https://github.com/ayushanand18/CSS501_Group1_Client/
git clone https://github.com/ayushanand18/CSS501_Group1_Server/
cd CSS501_Group1_Client/rpc_lib/
sudo cp -rf include/rpc /usr/local/include/
sudo cp lib/librpc.a /usr/local/lib/
cd ../../CSS501_Group1_Server && mkdir uploaded_files/
make clean build
cd ../CSS501_Group1_Client && mkdir downloads/
make clean build
```

## Next Steps(/Second-time run)
> Follow-on (or directly execute this if you are running not for the first time).
```shell
cd ../CSS501_Group1_Server
make run
```
and on a different terminal
```shell
cd CSS501_Group1_Client/
make run
```

## Errors
> If you get some of the below errors during execution of the above steps, please do the following.
* `Command 'make' not found, but can be installed with:`
    + install with `sudo apt install make`
* `src/server.cpp:14:10: fatal error: openssl/md5.h: No such file or directory`
    + install with `sudo apt-get install libssl-dev`
* `/usr/bin/ld: cannot find -lz: No such file or directory`
    + `sudo apt-get install zlib1g-dev`
* Make sure to run `sudo apt update` before running any of these fixes.
