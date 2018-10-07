# Skyr URL

## Status

[![License](
    https://img.shields.io/badge/license-boost-blue.svg "License")](
    https://github.com/cpp-netlib/url/blob/master/LICENSE_1_0.txt)
[![Travis Build Status](
    https://travis-ci.org/cpp-netlib/url.svg?branch=master "Build Status")](
    https://travis-ci.org/cpp-netlib/url)
[![AppVeyor ](https://ci.appveyor.com/api/projects/status/hc10ha6ugl6ea90u?svg=true)](
    https://ci.appveyor.com/project/glynos/url-3aeqd)

    
## Introduction

This library provides:

* A ``skyr::url`` class that implements a generic URL parser,
  compatible with the [WhatWG URL specification](https://url.spec.whatwg.org/#url-class)
* URL serialization and comparison
* Percent encoding and decoding functions
* IDNA and Punycode functions for domain name parsing

## Building the project

This project requires the availability of a C++17 compliant compiler
and standard library.

### Building with `CMake` and `Make`

From a terminal, execute the following sequence of commands:

```bash
> mkdir _build
> cd _build
> cmake ..
> make -j4
```

### Building with `CMake` and `Visual C++`

```bash
> mkdir _build
> cd _build
> cmake ..
```

The Visual Studio solution is available in `Skyr.sln`.

### Running the tests

```bash
> ctest
```

## Examples

These examples are based on the
[WhatWG API specification](https://url.spec.whatwg.org/#example-5434421b)

To build the examples, run `cmake` as follows:

```bash
> cmake .. -DSkyr_BUILD_EXAMPLES=ON
```

### Creating a URL without a base URL

This example parses a string, "https://example.org/💩",
without using a base URL:

```c++
#include <skyr/url.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  auto url = skyr::make_url("http://example.org/\xf0\x9f\x92\xa9");
  std::cout << url.value().pathname() << std::endl;
}
```

Gives the output: `/%F0%9F%92%A9`

### Creating a non-absolute URL without a base URL

This gives an error if the input, "/🍣🍺", is not an
*absolute-URL-with-fragment-string*:

```c++
#include <skyr/url.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  auto url = skyr::make_url("\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
  if (!url) {
    std::cerr << "Parsing failed: " << url.error().message() << std::endl;
  }
}
```

This gives the output: `Parsing failed: Not an absolute URL with fragment`

### Creating a non-absolute URL with a base URL

This example parses a string, "🏳️‍🌈", using a base URL, 
"https://example.org/":

```c++
#include <skyr/url.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  auto base = skyr::make_url("https://example.org/");
  auto url = skyr::make_url(
    "\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base.value());
  if (url) {
    std::cout << url.value().href() << std::endl;
  }
}
```

This gives the output: `https://example.org/%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88`

## Installation

```bash
> cmake .. DCMAKE_INSTALL_PREFIX=$PREFIX
> make -j4
> make test      # optional
> make install
```

Where `$PREFIX` is the location where you want to install the
library. Depending on the location of `$PREFIX`, you may need to run
the install command using as an administrator (on Linux as `sudo`).

## Dependencies

This library uses [optional](https://github.com/TartanLlama/optional),
[expected](https://github.com/TartanLlama/expected) and [utfcpp](https://github.com/nemtrif/utfcpp).

The tests are built using [Google Test](https://github.com/google/googletest).

## Requirements

This library has been tested using the following platforms and
compilers:

Linux:

* GCC 7
* GCC 8
* Clang 6

MacOS:

* Clang 6

Windows:

* Microsoft Visual C++ 2017

## License

This library is released under the Boost Software License (please see
http://boost.org/LICENSE_1_0.txt or the accompanying LICENSE_1_0.txt
file for the full text.

## Why *skyr*?

This name was chosen by a random project name generator, which
itself was randomly chosen.

## Contact

Any questions about this library can be addressed to the cpp-netlib
[developers mailing list]. Issues can be filed using Github at
http://github.com/cpp-netlib/uri/issues.

[developers mailing list]: cpp-netlib@googlegroups.com
