# stutils
A Utils Library.

## Features
* Logging
* Configure
* Data structure, eg. Stack, Queue, Heap
* Socket wrapper
* Dict and Alphabet

## Usage
First, Build the library,
 
```shell
$ git clone https://github.com/wantee/stutils.git
$ cd stutils/src
$ make
```

Then, set the proper environment variables,

```shell
$ export C_INCLUDE_PATH=$PATH_TO_STUTILS/include/stutils/:$C_INCLUDE_PATH
$ export CPLUS_INCLUDE_PATH=$PATH_TO_STUTILS/include/stutils/:$CPLUS_INCLUDE_PATH
$ export LIBRARY_PATH=$PATH_TO_STUTILS/stutils/lib/:$LIBRARY_PATH
$ export LD_LIBRARY_PATH=$PATH_TO_STUTILS/stutils/lib/:$LD_LIBRARY_PATH
```

For Mac OSX, the last line should be

```
$ export DYLD_LIBRARY_PATH=$PATH_TO_STUTILS/stutils/lib/:$DYLD_LIBRARY_PATH
```
