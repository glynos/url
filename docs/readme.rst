Skyr URL
========

Status
------

|License| |Travis Build Status| |AppVeyor Build Status|

Introduction
------------

This library provides:

-  A ``skyr::url`` class that implements a generic URL parser,
   compatible with the `WhatWG URL
   specification <https://url.spec.whatwg.org/#url-class>`__
-  URL serialization and comparison
-  Percent encoding and decoding functions
-  IDNA and Punycode functions for domain name parsing

Building the project
--------------------

This project requires the availability of a C++17 compliant compiler and
standard library.

Building with ``CMake`` and ``Make``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

From a terminal, execute the following sequence of commands:

.. code:: bash

    > mkdir _build
    > cd _build
    > cmake ..
    > make -j4

Building with ``CMake`` and ``Visual C++``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: bash

    > mkdir _build
    > cd _build
    > cmake ..

The Visual Studio solution is available in ``Skyr.sln``.

Running the tests
~~~~~~~~~~~~~~~~~

.. code:: bash

    ctest

Examples
--------

Creating a URL without a base URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parses a string, "https://example.org/💩", without using a base URL:

.. code:: cpp

    #include <skyr/url.hpp>
    #include <iostream>

    int main(int argc, char *argv[]) {
      auto url = skyr::make_url("http://example.org/\xf0\x9f\x92\xa9");
      std::cout << url.value().pathname() << std::endl;
    }

Gives the output: ``/%F0%9F%92%A9``

Creating a non-absolute URL without a base URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This gives an error if the input, "/🍣🍺", is not an
*absolute-URL-with-fragment-string*:

.. code:: cpp

    #include <skyr/url.hpp>
    #include <iostream>

    int main(int argc, char *argv[]) {
      auto url = skyr::make_url("\xf0\x9f\x8d\xa3\xf0\x9f\x8d\xba");
      if (!url) {
        std::cerr << "Parsing failed: " << url.error().message() << std::endl;
      }
    }

This gives the output:
``Parsing failed: Not an absolute URL with fragment``

Creating a non-absolute URL with a base URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parses a string, "🏳️‍🌈", using a base URL, "https://example.org/":

.. code:: cpp

    #include <skyr/url.hpp>
    #include <iostream>

    int main(int argc, char *argv[]) {
      auto base = skyr::make_url("https://example.org/");
      auto url = skyr::make_url("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88", base.value());
      if (url) {
        std::cout << url.value().href() << std::endl;
      }
    }

This gives the output:
``https://example.org/%F0%9F%8F%B3%EF%B8%8F%E2%80%8D%F0%9F%8C%88``

Why *skyr*?
-----------

This name was chosen by a random project name generator, which itself
was randomly chosen.

Dependencies
------------

This library uses
`optional <https://github.com/TartanLlama/optional>`__,
`expected <https://github.com/TartanLlama/expected>`__ and
`utfcpp <https://github.com/nemtrif/utfcpp>`__.

The tests are built using `Google
Test <https://github.com/google/googletest>`__.

Requirements
------------

This library has been tested using the following platforms and
compilers:

Linux:

-  GCC 7
-  GCC 8
-  Clang 6

MacOS:

-  Clang 6

Windows:

-  Microsoft Visual C++ 2017

License
-------

This library is released under the Boost Software License (please see
http://boost.org/LICENSE\_1\_0.txt or the accompanying LICENSE\_1\_0.txt
file for the full text.

Contact
-------

Any questions about this library can be addressed to the cpp-netlib
`developers mailing list <cpp-netlib@googlegroups.com>`__. Issues can be
filed using Github at http://github.com/cpp-netlib/uri/issues.

.. |License| image:: https://img.shields.io/badge/license-boost-blue.svg
   :target: https://github.com/glynos/url/blob/master/LICENSE_1_0.txt
.. |Travis Build Status| image:: https://travis-ci.org/glynos/url.svg?branch=master
   :target: https://travis-ci.org/glynos/url
.. |AppVeyor Build Status| image:: https://ci.appveyor.com/api/projects/status/8y5sd2k1nytxeya0?svg=true
   :target: https://ci.appveyor.com/project/glynos/url
