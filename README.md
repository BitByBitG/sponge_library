# Sponge Library

A personal C++ library for competitive programming.

## Requirements

* C++20 or later
* GCC 13+ recommended
* Windows for the helper tools in `bin/`

## Usage

Add `include/` to the compiler's include path:

```bash
g++ main.cpp -std=c++20 -O2 -I/path/to/sponge_library/include
```

Then include the modules you need:

```cpp
#include <sponge/segtree.hpp>
```

## Tools

Prebuilt Windows executables are available in `bin/`.

It is recommended to add `sponge_library/bin` to `PATH` so that the tools can be used directly from anywhere.

## License

This project is licensed under the GNU General Public License v3.0.
