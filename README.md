# Sponge Library

A personal C++ library for competitive programming.

## Requirements

* C++20 or later
* GCC
* Windows for the helper tools in `bin/`

## Structure

```text
sponge_library/
├── include/
│   └── sponge/       # Library headers
├── src/              # Source code of helper tools
├── bin/              # Prebuilt helper tools
└── LICENSE
```

The library contains implementations of commonly used algorithms and data structures, including graph algorithms, flows, segment trees, trees, modular arithmetic, matrices, polynomials, strings, and more.

## Usage

Add `include/` to the compiler's include path:

```bash
g++ main.cpp -std=c++20 -O2 -I/path/to/sponge_library/include
```

Then include the modules you need:

```cpp
#include <sponge/core.hpp>
#include <sponge/segtree.hpp>
#include <sponge/modint.hpp>

using namespace sponge;
```

Or simply include the whole library:

```cpp
#include <sponge/all.hpp>

using namespace sponge;
```

## Tools

Prebuilt Windows executables are available in `bin/`.

It is recommended to add `sponge_library/bin` to `PATH` so that the tools can be used directly from anywhere.

### compile

`compile` is a small wrapper around `g++` for quickly compiling competitive programming code.

```bash
compile main.cpp
```

The `.cpp` extension can be omitted:

```bash
compile main
```

It automatically locates `../include` relative to `compile.exe`, so the library can be moved without modifying any absolute paths.

Additional arguments are passed to `g++`:

```bash
compile main -DDEBUG
```

The resulting executable is written next to the source file.

### run

`run` is used to quickly execute a compiled solution.

```bash
run main
```

It is intended for convenient local testing during competitive programming.

### expander

`expander` expands `#include <sponge/...>` directives recursively and produces a standalone source file suitable for submission to online judges.

```bash
expander main.cpp
```

Use

```bash
expander --help
```

for all available options.

## Typical Workflow

Write a solution using Sponge Library:

```cpp
#include <sponge/all.hpp>
using namespace sponge;

int main()
{
    // ...
}
```

Compile it:

```bash
compile main
```

Test it locally:

```bash
run main
```

Before submitting, expand the library headers:

```bash
expander main.cpp
```

## License

This project is licensed under the MIT License.
