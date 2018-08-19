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
auto url = skyr::make_url("\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
if (!url) {
  std::cerr << "Parsing failed: " << url.error().message() << std::endl;
}
```

Gives the output: `Parsing failed: Not an absolute URL with fragment`

### Creating a non-absolute URL with a base URL

```c++
auto base = skyr::make_url("https://example.org/");
auto url = skyr::make_url("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base.value());
if (url) {
  std::cout << url.value().href() << std::endl;
}
```

Gives the output: `https://example.org/%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88`

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
