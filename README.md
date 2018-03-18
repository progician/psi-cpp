Private Set Intersection in C++
===============================

This project is an exercise in the Private Pet Intersection with Oblivious Function Evaluation, written in C++. There's a closely equivalent of the same project implemented in Common Lisp. If you are interested in understanding how the PSI algorithm would work with homomorphic encryption method, check it out.


Dependencies
------------

 * CMake 3.9
 * C++14 capable C++ compiler


Building
--------

The CryptoCom library is by default a header only library and requires no building. Installation should be as straightforward as to set it up in the include directory path. However, to understand its usage you might want to check out the unit tests written to develop and to document the behaviour of the library.

While the project is using CMake as a build system, CTest so far doesn't integrate well with Cath unit testing framework so at the moment you can not run it with CTest. You can, however, just run it via calling the check target.

    $ cmake . -B build-release -DCMAKE_BUILD_TYPE=Release
    $ ninja -C build-release
    $ ninja check -C build-release
