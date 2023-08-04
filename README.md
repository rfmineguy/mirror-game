# mirror-lib
A simple library that simulates the way light reflects off of mirror surfaces meant to simple to use.

# Building
```
shell
$ mkdir build
$ cd build
$ cmake .. -G Ninja
$ ninja
```
The built example executable is `build/mirrors`
The built static library is `build/libmirrorlib.a`

# Libraries used
The only library that was used is automatically handled by CMake, but for the sake of listing it:
  - [raylib](https://github.com/raysan5/raylib) by raysan5
