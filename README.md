# 🎉 Welcome to MLLIF!

***MLLIF*** is a ***M***LIR based ***L***anguage to ***L***anguage ***I***nteroperability ***F***lyover -
with a goal to integrate API of all programming languages.

For details, See [docs](https://mllif.sharp0802.com)!

## 📦 Dependencies

|                      |             Origin              |    Tested on     |
|:--------------------:|:-------------------------------:|:----------------:|
| LLVM/Clang (ClangIR) | https://github.com/llvm/clangir | git (2025-01-18) |

* **Tested on *git*** means it tested with submodule or local-clone of master branch of it

## 🧐 Briefly, How does this work?

> [!WARNING]
> Currently, CMake script supports clang-based languages only
> because this method depends on `mllif-cir`.
> For details, See [docs](https://mllif.sharp0802.com).

You can use CMake function `add_mllif_library` in `cmake/UseMLLIF.cmake`:

```cmake
add_mllif_library(<target> <language> <sources>...)
```

as:

```cmake
add_mllif_library(my-project CXX main.cxx foo.cxx)
```

> [!NOTE]
> language should be language identifier in CMake

It generates platform-dependent shared-object file with MSM file
(MLLIF-symbol-model; it's just XML :P).

Let's assume target created with this command uses language ***'A'***.
Bridge from host-language (***B***) to servant-language (***A***) involves from this MSM file.
Then, with FFI in each language, runtime of ***B*** calls bridge between ***A*** and ***B***.
And that bridge calls runtime of ***A*** (created shared-object).

By this approach, Each language can call each other in single process,
without any additional communication technics (such as TCP, Unix socket, shared memory, etc...).

For details, See [docs](https://mllif.sharp0802.com)!
