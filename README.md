# sweetPy

sweetPy is a seamless binding library, intended to export your C++ code with ease into Python 2.7.

sweetPy will allow you to export your C++ code in an object oriented style, with out the need to learn python’s data model and C-API.

## Getting Started

sweetPy is written in C++14, it is officially based upon the gnu compiler, version - 5.4.0.

Its using CMake as its higher echelon build system, version 3.0 and higher.

The code is fully portable, but officially it was only tested in linux environment, tho a windows build is also a possibility, only limited by a matching support of visual c++.

### Prerequisites

Gnu - version 5.0 and higher.

CMake - version 3.0 and higher.

Python - version 2.7.

GoogleTest - version 1.8.0 and higher - not mandatory.

GoogleTesst will be fetched automatically on need.

### Installing

sweetPy instalment is based upon two phases:
1. Resolving all 3rd party dependencies.
2. Compilation of sweetPy, its examples and if requested - its tests.

#### 3RD Party Dependencies instalment

| Argument  | Description |
| ------------- | ------------- |
| pyCppConn_3RD_PARTY_INSTALL_STEP - Mandatory  | 3rd parties installation step |
|pyCppConn_Test_Support - Optional| Will install the google test package  |

In sweetPy root directory:
```
cmake . -DpyCppConn_3RD_PARTY_INSTALL_STEP=ON -DpyCppConn_Test_Support=ON && make
```

#### Compilation phase

| Argument  | Description |
| ------------- | ------------- |
| pyCppConn_COMPILE_STEP - Mandatory  | Will compile sweetPy and its example |
|pyCppConn_Test_Support - Optional| Will compile sweetPy tests  |

In sweetPy root directory:
```
cmake . -DpyCppConn_COMPILE_STEP=ON -DpyCppConn_Test_Support=ON && make
```

The binary product of sweetPy is a shared object file located at - 
In sweetPy root directory:
```
./bin/libpyCppConn.so
```

## Running the tests

In order to run sweetPy tests, just run the following command, from sweetPy root directory:
```
./Tests/bin/pycppconnTests
```

## SweetPy supporting capabilities

sweetPy supports the following C++ language capabilities, into python:

1. Exporting user types.
1. Exporting Enums.
3. User types supports:
- User defined constructor.
- User defined destructors.
- Member functions (Not including overloading as of now).
- Members - both const and not (for read and write permission).
- static member functions.
4. Reference types.
- Invocation upon reference types.
- Accessing reference types members.
5. Seamless transition between python builtin types into your C++ code.
6. Seamless transition between C++ POD types and user defined types into python.
