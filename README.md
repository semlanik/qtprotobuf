# QtProtobuf

gRPC and Protobuf generator and bindings for Qt framework
> see [Protobuf](https://developers.google.com/protocol-buffers) and [gRPC](https://grpc.io/) for more information

QtProtobuf provides Qt-native support of Google protocol buffers. Generated code doesn't depend on any framework except Qt framework. QtProtobuf uses existing Qt meta-object information to access object fields and supports most of features of original C++ protocol buffers. It's not just a wrapper arround protocol buffers, but pure Qt generator with own serializers. Our target to provide deep Qt, protocol buffers and gRPC intergration.

### Test results

| Branch | Results | Support |
| --- | --- | --- |
| [master](https://github.com/semlanik/qtprotobuf/tree/master) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=master) | :heavy_check_mark: |
| [0.5](https://github.com/semlanik/qtprotobuf/tree/0.5) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.5) | :heavy_check_mark: |
| [0.4](https://github.com/semlanik/qtprotobuf/tree/0.4) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.4) | :heavy_check_mark: |
| [0.3](https://github.com/semlanik/qtprotobuf/tree/0.3) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.3) | :heavy_check_mark: |
| [0.2](https://github.com/semlanik/qtprotobuf/tree/0.2) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.2) |  |
| [0.1.0](https://github.com/semlanik/qtprotobuf/tree/0.1.0) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.1.0) |  |


# Table of contents

[API Reference](https://semlanik.github.io/qtprotobuf)

[Linux Build](#linux-build)

[Windows Build](#windows-build)

[Cross-compiling](#cross-compiling)

[Usage](#usage)

[Integration with CMake project](#integration-with-cmake-project)

[Integration with qmake project](#integration-with-qmake-project)

[Generated and QtProtobuf types registation](#generated-and-qtprotobuf-types-registation)

[Code exceptions](#code-exceptions)

[QtProtobuf development](#qtprotobuf-development)

## Tutorials

[QtProtobuf Client Tutorial](https://semlanik.github.io/qtprotobuf/clienttutorial.html)

# Build

## Build options

*QT_PROTOBUF_MAKE_COVERAGE* - if **TRUE/ON**, enables gcov intergration, to collect code coverage reports(usefull for developers). **FALSE** by default.

*QT_PROTOBUF_MAKE_TESTS* - if **TRUE/ON**, enables tests for QtProtobuf. **TRUE** by default.

*QT_PROTOBUF_MAKE_EXAMPLES* - if **TRUE/ON**, enables built-in examples. **TRUE** by default.

*QT_PROTOBUF_NATIVE_GRPC_CHANNEL* - if **TRUE/ON**, enables build of an additional channel wrapping native gGRPC C++ library (**Note:** grpc++ library is required).

*BUILD_SHARED_LIBS* - if **TRUE/ON**, enables shared libraries build, **FALSE** by default, static libraries build is performed.

> **Note:** In case if you use static QtProtobuf in your non-cmake/-qmake build system, you additionaly **need manually** add QT_PROTOBUF_STATIC compiler definition.

*QT_PROTOBUF_FIELD_ENUM* - if **TRUE/ON**, adds enumeration with message fields for generated messages in QtProtobufTypes and QtProtobufWellKnownTypes libraries. **FALSE** by default.

## Linux Build
### Prerequesties

Check installation of following packages in your system:

- cmake 3.14 or higher
- Qt 5.12.4 or higher
- protobuf 3.6.0 or higher (might be used from submodule)
- golang 1.10 or higher

Optional:

- grpc 1.15.0 or higher (might be used from submodule)

>**Note:** Older versions could be supported as well but not tested.

#### Option 1: Use system libraries

##### For Ubuntu 19.10 or higher
Install dependencies:

```bash
sudo apt-get
    build-essential \
    cmake \
    golang \
    wget \
    libdbus-1-3 \
    libfreetype6 libfontconfig libx11-6 \
    libgl1-mesa-dev \
    libsm6 \
    libice6 \
    libxext6 \
    libxrender1 \
    doxygen \
    qt5-default \
    qtdeclarative5-private-dev \
    qtbase5-private-dev \
    protobuf-compiler \
    libprotoc-dev \
    protobuf-compiler-grpc \
    libgrpc++-dev \
    libgrpc-dev \
    libgtest-dev
```

##### For OpenSUSE 15.2 or higher
Install dependencies:

```bash
sudo zypper in go \
    wget \
    cmake \
    libXrender1 \
    libXext6 \
    libSM6 \
    Mesa-libGL1 \
    libfreetype6 \
    libX11-6 \
    libdbus-1-3 \
    fontconfig \
    libICE6 \
    gcc-c++ \
    Mesa-devel \
    libgthread-2_0-0 \
    libqt5-qttools \
    protobuf-devel \
    grpc-devel \
    libqt5-qtdeclarative-devel \
    libqt5-qtbase-devel \
    libqt5-qtdeclarative-private-headers-devel \
    libqt5-qtbase-private-headers-devel \
    rpm-build
sudo zypper --non-interactive in --type pattern devel_C_C++
```

#### Option 2: All-in-one build

If required versions of libraries are not found in your system, you may use all-in-one build procedure for prerequesties.

Manually install Qt version you need [](https://www.qt.io/download).

Update submodules to fetch 3rdparty dependencies:

```bash
git submodule update --init --recursive
```

>**Note:** All installation rules are disabled for all-in-one build.

#### Option 3: Use as sub-module

QtProtobuf build procedure is designed to use QtProtobuf as sub-project.

>**Note:** This option only supported by projects that use CMake as build tool.

Add QtProtobuf as git submodule in your Project folder:

```bash
git submodule add https://github.com/semlanik/qtprotobuf.git qtprotobuf
git submodule init qtprotobuf
git submodule update qtprotobuf

#(Optional) You also may initialize all internal QtProtobuf sub-modules for all-in-one build using steps below:
cd qtprotobuf
git submodule update --init --recursive
```

Next add QtProtobuf subproject to your CMake tree, edit CMakeLists.txt:

```cmake
...
add_subdirectory("qtprotobuf")
...
```

QtProtobuf and dependencies from source tree will be built within your Project.

>**Note:** All installation rules of QtProtobuf project are disabled for sub-project build.

### Build

```bash
git submodule init src/protobuf/3rdparty/microjson #microjson is part of build and installation tree. Made this to simplify dependency management, but you always may use system microjson if installed.
git submodule update src/protobuf/3rdparty/microjson
mkdir build
cd build
cmake .. [-DCMAKE_PREFIX_PATH="<path/to/qt/installation>/Qt<qt_version>/<qt_version>/gcc_64/lib/cmake"]
cmake --build . [--config <RELEASE|DEBUG>] -- -j<N>
```

### Packaging

QtProtobuf has packaging support based on CPack.

#### .deb

You can create .deb package for debian-like operating systems, using commands below:

```bash
mkdir build
cd build
cmake .. -DQT_PROTOBUF_MAKE_TESTS=FALSE -DQT_PROTOBUF_MAKE_EXAMPLES=FALSE
cpack -G DEB ..
```

>**Note:** Only tested on Ubuntu 19.10

#### .rpm

You can create .rpm package for rpm-based operating systems, using commands below:

```bash
mkdir build
cd build
cmake .. -DQT_PROTOBUF_MAKE_TESTS=FALSE -DQT_PROTOBUF_MAKE_EXAMPLES=FALSE
cpack -G RPM ..
```

>**Note:** Only tested on OpenSUSE 15.2

## Windows Build
### Prerequesties

Download and install:

- Qt 5.12.3 or higher [1](https://download.qt.io/official_releases/qt/)
- cmake-3.6 or higher [2](https://cmake.org/download/)
- Strawberry perl 5.28 [3](http://strawberryperl.com/)
- GoLang 1.10 or higher [4](https://golang.org/dl/)
- Yasm 1.3 or higher [5](http://yasm.tortall.net/Download.html)
- Actual Visual Studio Compiler with cmake support that required by Qt [6](https://visualstudio.microsoft.com/downloads/)

>**Note:** All applications should be in PATH

Update submodules to fetch 3rdparty dependencies:

```bash
git submodule update --init --recursive
```

### Build

Open Qt MSVC command line and follow steps:

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" <x86|x64>
cd <directory with qtprotobuf project>
mkdir build
cd build
cmake ..
cmake --build . [--config <RELEASE|DEBUG>] -- /m:<N>
```

## Cross-compiling

>**Note:** Cross-compiling support is available since version 0.6.0

Cross compiling is only supported in the prefixed builds of QtProtobuf. It's split into three parts:

- Build and install the Qt framework for the target system (Instructions could be found in the documentation for the Qt Framework)
- Build for the host system
- Build for the target system

### Build for the host system

Build for the host system must be prefixed, that means you should install it to the system or the temporary directory,
using the following steps:

```bash
mkdir build-host
cd build-host
cmake </path/to/qtprotobuf/sources> -DQT_PROTOBUF_MAKE_TESTS=OFF -DQT_PROTOBUF_MAKE_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=</host/sysroot/prefix>
cmake --build . -- -j<N>
cmake --install .
```

### Build for the target system

To build QtProtobuf correctly for the target system you need to specify compiler and other build essentials in the CMake toolchain file.
Also it's necessary to specify the QT_PROTOBUF_HOST_PATH variable, that is used to find a host binary of the QtProtobuf generator.

:warning: **IMPORTANT** The QT_PROTOBUF_HOST_PATH variable shall point to the *<lib|lib32|lib64>/cmake* directory of the host QtProtobuf installation.

Typical toolchain file looks as following:

```cmake
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_SYSROOT /usr/armv7a-hardfloat-linux-gnueabi/)

set(CMAKE_C_COMPILER /usr/bin/armv7a-hardfloat-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/armv7a-hardfloat-linux-gnueabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER) # Important to look programms in the host sysroot only
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH) # Important to look packages in both the host and target sysroots
```

Using the below steps configure and build the target QtProtobuf:

>**Note:** Use values of the expressions same as in the *Build for the host system* step.

```bash
mkdir build-host
cd build-host
cmake </path/to/qtprotobuf/sources> -DQT_PROTOBUF_MAKE_TESTS=OFF -DQT_PROTOBUF_MAKE_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX="</target/sysroot/prefix>" -DCMAKE_TOOLCHAIN_FILE="</path/to/toolchain.cmake>" -DQT_PROTOBUF_HOST_PATH="</host/sysroot/prefix>/<lib|lib32|lib64>/cmake" -DCMAKE_PREFIX_PATH="</target/sysroot/cross-compiled/qt>"
```

### Build your project using the target QtProtobuf

>**Note:** Only the CMake-based user projects have a cross-compiling support.

When you build your own project using the target QtProtobuf, it's recommented to use the same toolchain file as you used to compile QtProtobuf.
You also still need to specify the QT_PROTOBUF_HOST_PATH variable to find the host QtProtobuf generator. Use the following steps as example:

```bash
mkdir build-clienttutorial
cd build-clienttutorial
cmake </path/to/qtprotobuf/sources/examples/clienttutorial> -DCMAKE_PREFIX_PATH="</target/sysroot/prefix>;</target/sysroot/cross-compiled/qt>" -DQT_PROTOBUF_HOST_PATH="</host/sysroot/prefix>/<lib|lib32|lib64>/cmake" -DCMAKE_TOOLCHAIN_FILE="</path/to/toolchain.cmake>"

```

If you build the shared version of the QtProtobuf libraries don't forget to copy them to the target filesystem before running the application.

## Conan build

>**Note:** QtProtobuf supports conan builds since version 0.4.0.

### Build QtProtobuf

Before build make sure that all sub-modules are deinitilized:

```bash
git submodule deinit -f --all
```

Build QtProtobuf:

```bash
mkdir -p build_conan
cd build_conan
conan source ..
conan install .. --build=microjson
conan build ..
```

### Conan project integration

QtProtobuf conan source packages are hosted on JFrog Bintray. Add remote to conan:

```bash
conan remote add semlanik https://api.bintray.com/conan/semlanik/libs
```

To intergrate QtProtobuf add it as a dependency to your conanfile.py:

```python
    ...
    requires = [
        ...
        "qtprotobuf/0.5.0@semlanik/stable",
        ...
    ]
    ...
```
>**Note:** You also may use QtProtobuf package is built localy or from other remote source. Look in conan documentation for details.

# Usage
## Direct usage of generator

```bash
[QT_PROTOBUF_OPTIONS="[SINGLE|MULTI]:QML:COMMENTS:FOLDER:FIELDENUM:EXTRA_NAMESPACE=<value>"] protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin/>qtprotobufgen --qtprotobuf_out=<output_dir> [-I/extra/proto/include/path] <protofile>.proto
```

### QT_PROTOBUF_OPTIONS

For protoc command you also may specify extra options using QT_PROTOBUF_OPTIONS environment variable and colon-separated format:

``` bash
[QT_PROTOBUF_OPTIONS="[SINGLE|MULTI]:QML:COMMENTS:FOLDER:FIELDENUM:EXTRA_NAMESPACE=<value>"] protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin/>qtprotobufgen --qtprotobuf_out=<output_dir> [-I/extra/proto/include/path] <protofile>.proto
```

Following options are supported:

*SINGLE* - enables single-file generation when for each *.proto* file single pair of *.h* *.cpp* files is generated

>**Note:** Enabled by default. Excluded by SINGLE

*MULTI* - enables multi-file generation when for each message separate pair of *.h* *.cpp*

>**Note:** Has higher priority than SINGLE

*QML* - enables QML code generation in protobuf classes. If is set QML-related code for lists and QML registration to be generated.

*COMMENTS* - enables comments copying from .proto files

*FOLDER* - enables folder-based generation

*FIELDENUM* - adds enumeration with message fields for generated messages.

## Integration with CMake project

You can integrate QtProtobuf as submodule in your project or as installed in system package. Add following line in your project CMakeLists.txt:

```cmake
...
find_package(QtProtobuf CONFIG REQUIRED COMPONENTS ProtobufGenerator Protobuf Grpc)
file(GLOB PROTO_FILES ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/path/to/protofile1.proto
 ${CMAKE_CURRENT_SOURCE_DIR}/path/to/protofile2.proto
 ...
 ${CMAKE_CURRENT_SOURCE_DIR}/path/to/protofileN.proto)
# Function below generates source files for specified PROTO_FILES,
# and link them to the MyTarget as static library
add_executable(MyTarget main.cpp) # Add your target here
qtprotobuf_generate(TARGET MyTarget
OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
PROTO_FILES ${PROTO_FILES})

```

Another option is to pre-specify expected generated headers to prevent dummy-parser mistakes

```cmake
...
set(GENERATED_HEADERS
# List of artifacts expected after qtprotobufgen job done.
# Usually it's full list of messages in all packages with .h header suffix
...
)
...
qtprotobuf_generate(TARGET MyTarget
OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
PROTO_FILES ${PROTO_FILES}
GENERATED_HEADERS ${GENERATED_HEADERS})
...
```

In case if you somehow avoided ```qtprotobuf_generate``` usage, you need manualy link QtProtobuf libraries used by project, e.g.:
```cmake
...
target_link_libraries(${TARGET} QtProtobuf::Protobuf QtProtobuf::Grpc QtProtobuf::ProtobufWellKnownTypes)
...
```

### CMake functions reference
#### qtprotobuf_generate

qtprotobuf_generate is cmake helper function that automatically generates STATIC library target from your .proto files

***Parameters:***

*TARGET* - name of you target that generated code archive will be linked to

*GENERATED_TARGET* - name that will be used for generated archive library target. It's usefull when you supposed to have multiple generated targets to be linked to single one.

*OUT_DIR* - output directory that will contain generated artifacts. Usually subfolder in build directory should be used

*GENERATED_HEADERS* - List of header files expected after generator job finished

*EXCLUDE_HEADERS* - List of header files to be excluded from pre-parsed list of expected header files (e.g. nested messages that are not supported by QtProtobuf generator)

*PROTO_FILES* - List of .proto files that will be used in generation procedure

***Options:***

*MULTI* - Enables multi-files generation mode. If provided in parameter list generator will create pair of header/source files for each message.

>**Note:** multi-files generation mode is defined as deprecated by QtProtobuf team, and might have poor support in future

*QML* - Enables QML code generation in protobuf classes. If provided in parameter list QML related code for lists and QML registration to be generated.

*COMMENTS* - Enables comments copying from .proto files. If provided in parameter list message and field related comments will be copied to generated header files.

*FOLDER* - Enables folder based generation. If provided in parameter list generator will place generated artifacts to folder structure according to package of corresponding .proto file.

>**Note:** enabled by default if MULTI option provided

*FIELDENUM* - Adds enumeration with message fields for generated messages.

*EXTRA_NAMESPACE <namespace>* - Wraps the generated code with the specified namespace. (EXPERIMETAL)

#### qtprotobuf_link_target

qtprotobuf_link_target is cmake helper function that links generated protobuf target to your binary. It's useful when you try to link generated target to shared library or/and to executable that doesn't utilize all protobuf generated classes directly from C++ code, but requires them from QML.

***Parameters:***

*TARGET* - name of target to link to

*GENERATED_TARGET* - protobuf generated target name

#### Useful definitions

*QT_PROTOBUF_EXECUTABLE* - contains full path to QtProtobuf generator

## Integration with qmake project

>**Note:** Available in QtProtobuf version >=0.2.0

QtProtobuf has limited qmake build procedures support. It's only available and tested on linux platforms. To use it in your qmake project, first you need build and install QtProtobuf as standalone project in your system paths:

```bash
mkdir build
cd build
cmake .. [-DCMAKE_PREFIX_PATH="<path/to/qt/installation>/Qt<qt_version>/<qt_version>/gcc_64/lib/cmake"] -DCMAKE_INSTALL_PREFIX=/usr -DQT_PROTOBUF_MAKE_TESTS=OFF -DQT_PROTOBUF_MAKE_EXAMPLES=OFF
cmake --build . [--config <RELEASE|DEBUG>] -- -j<N>
sudo cmake --build . [--config <RELEASE|DEBUG>] --target install
```

Commands above will install qt protobuf into you system paths prefixed with */usr* folder.

Once build and installation is finished, you may use QtProtobuf and QtGrpc in qmake project by adding following lines in .pro file:

```bash
QT += protobuf #for protobuf libraries support
QT += grpc #for grpc libraries support
```
To generate source code and link it to you project use predefined *qtprotobuf_generate* function

### qmake functions reference
#### qtprotobuf_generate([generate_qml=false])

qtprotobuf_generate is qmake helper [test function](https://doc.qt.io/qt-5/qmake-language.html#test-functions) that generates QtProtobuf source code based on files provided by PROTO_FILES global context variable

***Parameters:***

*generate_qml* - Enables/disables QML code generation in protobuf classes. If set to `true` QML-related code for lists and QML registration to be generated.


>**Note:** see examples/qmakeexample for details

## Generated and QtProtobuf types registation

To enable QtProtobuf project you need to register protobuf types. It's good practice to make it in 'main' function.

```cpp
...
#include <QtProtobufTypes>
...
int main(int argc, char *argv[])
{
QtProtobuf::qRegisterProtobufTypes();
... //Qt application initialization and run
}
```

## Code exceptions

If any prohibited Qt/QML keyword is used as field name, generator appends '*_proto*' suffix to generated filed name. It's required to omit overloading for example QML reserved names like '*id*' or '*objectName*'.

E.g. for message:
```
message MessageReserved {
    sint32 import = 1;
    sint32 property = 2;
    sint32 id = 3;
}
```

Following properties will be generated:
```cpp
...
    Q_PROPERTY(QtProtobuf::sint32 import_proto READ import_proto WRITE setImport_proto NOTIFY import_protoChanged SCRIPTABLE true)
    Q_PROPERTY(QtProtobuf::sint32 property_proto READ property_proto WRITE setProperty_proto NOTIFY property_protoChanged SCRIPTABLE true)
    Q_PROPERTY(QtProtobuf::sint32 id_proto READ id_proto WRITE setId_proto NOTIFY id_protoChanged SCRIPTABLE true)
...

```

>**Note:** List of exceptions might be extended in future releases.

# QtProtobuf development
## Running tests

```bash
cd <build directory>
ctest
```

## Documentation generation

Project uses doxygen for documentation generation.

### Ubuntu

Install doxygen and graphviz packages:
```
sudo apt-get install -y doxygen graphviz
```

### Windows

Install doxygen and graphviz packages:
* [doxygen](http://www.doxygen.nl/download.html)
* [graphviz](https://graphviz.gitlab.io/_pages/Download/Download_windows.html)

### Generation

You can generate documentation for actual version using following commands:

```bash
mkdir build
cd build
cmake ..
cmake --build . --target doc
```
