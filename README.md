## Сборка

### cmake >= 3.21 + Ninja

Debug:

```
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Release:

```
cmake --preset release
cmake --build --preset release
ctest --preset release
```

### cmake < 3.21 + GNU Makefiles

Debug:

```
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
ctest --test-dir build/debug
```

Release:

```
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release
ctest --test-dir build/release
```