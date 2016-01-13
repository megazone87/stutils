# stutils (v0.0.0)
A Utils C Library.

[![Build Status](https://travis-ci.org/wantee/stutils.svg)](https://travis-ci.org/wantee/stutils)
[![License](http://img.shields.io/:license-mit-blue.svg)](https://github.com/wantee/stutils/blob/master/LICENSE)

## Features
* Logging
* Configure
* Data structure, eg. Stack, Queue, Heap
* Socket wrapper
* Dict and Alphabet
* Semaphore

## Build
### From git repo
 
```shell
$ git clone https://github.com/wantee/stutils.git
$ cd stutils
$ ./autogen.sh
$ configure --prefix=$YOUR_INSTALL_PATH
$ make
$ make check
$ make install
```

### From tarball
 
```shell
$ curl -sL clone https://github.com/wantee/stutils/archive/stutils-0.0.0.tar.gz
$ cd stutils-0.0.0
$ configure --prefix=$YOUR_INSTALL_PATH
$ make
$ make check
$ make install
```

## License

**stutils** is open source software licensed under the MIT License. See the `LICENSE` file for more information.

