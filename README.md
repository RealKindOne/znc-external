# ZNC - External Third-Party Modules

This is in a PROOF OF CONCEPT state. It works from my testing.

## What is this?

This is a collection of third-party ZNC modules with integration tests.

## Why?

Changes in ZNC can break modules. Having integration tests can help prevent that.

## Directions

You must globally install ZNC.

```
git clone https://github.com/RealKindOne/znc-external.git --recursive
```

Place custom module in `modules` folder.

Copy/paste `example_test.cpp`, rename into `MODULE-NAME-HERE_test.cpp`

Edit `MODULE-NAME-HERE_test.cpp`

Add `MODULE-NAME-HERE_test.cpp` in `tests\CMakeFiles.txt`


```
cmake -S . -DZNC_BIN_DIR=/usr/bin -DZNC_INCLUDE_DIR=/usr/include
```

Compile and run all existing tests.

```
make && tests/znc-external-tests
```

You can limit the tests for example only run the `example_test.cpp`:

```
make && tests/znc-external-tests --gtest_filter=ZNCTest.ExampleModule
```

## Cygwin

You must run this command for Cygwin:

```
export ZNC_QT_VER=5
```

## Limits

Does **NOT** test globally installed modules. Only the copies in the `modules/` folder.

## TODO 

Require 100% test coverage for modules?

CXXFLAGS="-fsanitize=address"

CI tests and stuff

Translations?

Deal with znctestconfig.h file

Improve README.md

Local /home/user/znc-dev install

AI was used only for CMake file - They need to be audited