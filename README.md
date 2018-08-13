# Skyr URL

[![Build Status](
    https://travis-ci.org/cpp-netlib/uri.png?branch=master "Build Status")](
    https://travis-ci.org/cpp-netlib/uri)

[![License](
    https://img.shields.io/badge/license-boost-blue.svg "License")](
    https://github.com/cpp-netlib/uri/blob/master/LICENSE_1_0.txt)

This library provides:

* A ``skyr::url`` class that implements a generic URI parser,
  compatible with [WhatWG URL specification](https://url.spec.whatwg.org/#url-class)
* Percent encoding and decoding functions
* A URI builder to build consistent URIs from parts, including
  case, percent encoding and path normalization


## Building the project

### Building with `CMake` and `Make`

```bash
mkdir _build
cd _build
cmake ..
make -j4
```

### Running the tests

```bash
make test
```

## Examples

### Creating a URL without a base URL

```c++
auto url = skyr::url("http://example.org/\xf0\x9f\x92\xa9");
std::cout << url.pathname() << std::endl;
```

### Creating an non-absolute URL without a base URL

```c++
auto url = skyr::url("/\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
if (!url) {
  std::cerr << "Parsing failed" << std::endl;
}
```

### Creating a non-absolute URL with a base URL

```c++
auto input = std::string("/\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
auto url = skyr::url(input, document.baseURI)
url.href(); // "https://url.spec.whatwg.org/%F0%9F%8D%A3%F0%9F%8D%BA"
```

## Dependencies

This library uses [optional](https://github.com/TartanLlama/optional)
and [expected](https://github.com/TartanLlama/expected).

The tests are built using [Google Test](https://github.com/google/googletest).

## Requirements

This library has been tested using the following compilers:

Linux:

* GCC 6
* GCC 7
* GCC 8
* Clang 5
* Clang 6

MacOS:

* Clang 5
* Clang 6

Windows:

* Visual Studio 2017

## License

This library is released under the Boost Software License (please see
http://boost.org/LICENSE_1_0.txt or the accompanying LICENSE_1_0.txt
file for the full text.

## Contact

Any questions about this library can be addressed to the cpp-netlib
[developers mailing list]. Issues can be filed using Github at
http://github.com/cpp-netlib/uri/issues.

[developers mailing list]: cpp-netlib@googlegroups.com
