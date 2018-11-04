# c2cpp
CalHacks 5.0 C to C++ program

## Getting Started
### Prerequisites
- cmake
- a C++ compiler (e.g. clang)
- a build system (e.g. ninja)

### Using lld
If you are building on Linux, you should consider installing lld and making it your default linker before preceeding. Once lld is installed, you can make it your default linker as follows:
```
ln -s /usr/bin/ld.lld /usr/bin/ld
```
See https://lld.llvm.org for more details.

### Installing
Clone the llvm and clang repositories.
```
mkdir ~/clang-llvm && cd ~/clang-llvm
git clone http://llvm.org/git/llvm.git
cd llvm/tools
git clone http://llvm.org/git/clang.git
cd clang/tools
git clone http://llvm.org/git/clang-tools-extra.git extra
```
Create and run the build files.
```
cd ~/clang-llvm
mkdir build && cd build
export CC=/usr/bin/clang && export CXX=/usr/bin/clang++
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../llvm
ninja
```
Test your build.
```
ninja check-all
```
Place the root directory of this project in `~/clang-llvm/llvm/tools/clang/tools/extra`.
The project can be built by running the build command again in `~/clang-llvm/build`.
