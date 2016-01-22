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

## Build
```shell
$ git clone https://github.com/wantee/stutils.git
$ cd stutils/src
$ make -j 4
$ make test
```

## Usage
1. Add include directory to CFLAGS.

  ```
CFLAGS += -I$(PATH_TO_STUTILS)/include
```

2. Include header(s) in your source file.

  ```
#include <stutils/st_xxx.h>
```

## Contributing

1. Fork it ( https://github.com/wantee/stutils.git )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## License

**stutils** is open source software licensed under the MIT License. See the `LICENSE` file for more information.
