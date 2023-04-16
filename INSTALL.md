In each step subdirectory, create an initial Makefile with a command like:

```shell
mkdir build && cd build && cmake ..
```

Then, try out the simple example:

```shell
make hello-world && ./hello-world
```

For tests, run:

```shell
make tests && ./tests
```
