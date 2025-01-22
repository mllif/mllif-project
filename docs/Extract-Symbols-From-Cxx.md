# Extract Symbols From C/C++

## Create New Project

We will create sample C++ project whose symbols are gonna be extracted:

```
CMakeLists.txt
main.cxx
```

## Configure with MLLIF

To use MLLIF in your CMake project,
You can include MLLIF's CMake toolkit:

```cmake
include(UseMLLIF)
```

Then, You can use `add_mllif_library` as below:

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject)

add_mllif_library(MyProject CXX main.cxx)
```

As you see, MLLIF project can be established with tree arguments:
target name, language id, source files:

```
add_mllif_library(<target> <language> <sources>)
```

> [!WARN]
> For C programs, Using CXX as language id is forced.
> Of course, You don't have to change any line of code.
> Just use CXX for your language id!

Also, you can modify compile-options or output directory, etc...

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyProject LANGUAGES CXX)

file(GLOB_RECURSE SOURCES *.cxx)
add_mllif_library(MyProject CXX ${SOURCES})
set_target_properties(MyProject PROPERTIES
        COMPILE_OPTIONS -std=c++26
)

set_target_properties(MyProject PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
```

> [!WARN]
> You cannot edit compile-option of it with `target_compile_options...` or `CMAKE_CXX_STANDARD` things!
> It's because `add_mllif_library` is implemented with `add_custom_target`.
> If you know how to interoperates them, Please make Pull Request!!!

## Tag it!

We will use sample C++ code as below:

```c++
class Foo {
    int a;
  public:
  
    [[mllif::export]]
    int A() const;
};

int Foo::A() const { return a; }
```

> [!NOTE]
> Inline implementation of functions in record-type will be ignored!
> These functions are inlined before MLIR-level.

``[[mllif::export]]`` attribute represents that marked symbol will be exported.
And, if the attribute annotates record-type (class, struct),
All public method of type will be exported.

So, This code and the code described before make same result:

```c++
class Foo [[mllif::export]] {
    int a;
  public:
  
    int A() const;
};

int Foo::A() const { return a; }
```

## Compilation, Symbols

You can get symbols and bridge simply.
Just build project target in CMake.
Then, You can get symbol-model file(\*.msm) and a dynamic library (bridge).

You can use this MSM file to create wrapper for a bridge with backend system.
For details, See backend tutorial with your favourite language!
