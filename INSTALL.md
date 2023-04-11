In each step subdirectory, create an initial Makefile with a command like:

```shell
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=17 ..
```
