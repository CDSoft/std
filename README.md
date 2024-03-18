# (Not so) Standard C library

`std` is a C library that provides some useful data containers.

It can be used in desktop applications as well as in more constrained
environment (e.g. real time critical applications).

# Usage

It consists of a single directory of C sources and header files. Embedding the
[`src`](src) directory in your projects should be enough.

# Tests

The [`tests`](tests) directory contains unit tests for all `std` containers.
They can be executed on Linux with [`ninja`](https://ninja-build.org/) and
`clang` with the address and undefined behaviour sanitizers:

``` sh
$ ninja test
```

# Documentation

The full documentation is available in Markdown and can be viewed with any good
editor or directly on GitHub: [std documentation](doc/std.md)

# License

    This file is part of std.

    std is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    std is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with std.  If not, see <https://www.gnu.org/licenses/>.

    For further information about std you can visit
    http://cdelord.fr/std
