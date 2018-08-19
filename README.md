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
* IDNA and Punycode functions for domain name parsing

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
auto url = skyr::make_url("http://example.org/\xf0\x9f\x92\xa9");
std::cout << url.value().pathname() << std::endl;
```

Gives the output: `/%F0%9F%92%A9`

### Creating an non-absolute URL without a base URL

```c++
auto url = skyr::make_url(U"/\u1F363\1F37A");
if (!url) {
  std::cerr << "Parsing failed" << std::endl;
}
```

Gives the output: `Parsing failed`

### Creating a non-absolute URL with a base URL

```c++
auto base = skyr::make_url("https://url.spec.whatwg.org/");
auto url = skyr::make_url(U"/\u1F363\1F37A", base.value());
if (url) {
  std::cout << url.value().href() << std::endl;
}
```

Gives the output: `https://url.spec.whatwg.org/%F0%9F%8D%A3%F0%9F%8D%BA`

## Dependencies

This library uses [optional](https://github.com/TartanLlama/optional)
and [expected](https://github.com/TartanLlama/expected).

The tests are built using [Google Test](https://github.com/google/googletest).

## Requirements

This library has been tested using the following compilers:

Linux:

* GCC 7
* GCC 8
* Clang 6

MacOS:

* Clang 6

## License

This library is released under the Boost Software License (please see
http://boost.org/LICENSE_1_0.txt or the accompanying LICENSE_1_0.txt
file for the full text.

## Contact

Any questions about this library can be addressed to the cpp-netlib
[developers mailing list]. Issues can be filed using Github at
http://github.com/cpp-netlib/uri/issues.

[developers mailing list]: cpp-netlib@googlegroups.com
