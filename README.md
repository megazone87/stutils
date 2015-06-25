# stutils
A Utils Library.

[![Build Status](https://travis-ci.org/wantee/stutils.svg)](https://travis-ci.org/wantee/stutils)
[![License](http://img.shields.io/:license-mit-blue.svg)](https://github.com/wantee/stutils/blob/master/LICENSE)

## Features
* Logging
* Configure
* Data structure, eg. Stack, Queue, Heap
* Socket wrapper
* Dict and Alphabet
* Semaphore

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

## License

**stutils** is open source software licensed under the MIT License. See the `LICENSE` file for more information.

