# QtProtobuf

gRPC and Protobuf generator and bindings for Qt framework
> see [Protobuf](https://developers.google.com/protocol-buffers) and [gRPC](https://grpc.io/) for more information

QtProtobuf provides Qt-native support of Google protocol buffers. Generated code doesn't depend on any framework except Qt framework. QtProtobuf uses existing Qt meta-object information to access object fields and supports most of features of original C++ protocol buffers. It's not just a wrapper arround protocol buffers, but pure Qt generator with own serializers. Our target to provide deep Qt, protocol buffers and gRPC intergration.

### Test results

| Branch | Results Linux | Results Windows |
| --- | --- | --- |
| [master](https://github.com/semlanik/qtprotobuf/tree/master) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=master) | ![](https://travis-ci.com/semlanik/qtprotobuf.svg?branch=master) |
| [0.1.0](https://github.com/semlanik/qtprotobuf/tree/0.1.0) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.1.0) | ![](https://travis-ci.com/semlanik/qtprotobuf.svg?branch=0.1.0) |
| [0.2](https://github.com/semlanik/qtprotobuf/tree/0.2) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.2) | ![](https://travis-ci.com/semlanik/qtprotobuf.svg?branch=0.2) |
| [0.3](https://github.com/semlanik/qtprotobuf/tree/0.3) | ![](https://github.com/semlanik/qtprotobuf/workflows/Test%20Verification/badge.svg?branch=0.3) | ![](https://travis-ci.com/semlanik/qtprotobuf.svg?branch=0.3) |


# Table of contents

[API Reference](https://semlanik.github.io/qtprotobuf)

[Linux Build](#linux-build)

[Windows Build](#windows-build)

[Usage](#usage)

[Integration with CMake project](#integration-with-cmake-project)

[Integration with qmake project](#integration-with-qmake-project)

[Generated and QtProtobuf types registation](#generated-and-qtprotobuf-types-registation)

[Code exceptions](#code-exceptions)

[QtProtobuf development](#qtprotobuf-development)

## Tutorials

[QtProtobuf Client Tutorial](https://semlanik.github.io/qtprotobuf/clienttutorial.html)

# Linux Build
## Prerequesties

Check installation of following packages in your system:
- cmake 3.6 or higher
- Qt 5.12.4 or higher
- protobuf 3.6.0 or higher
- grpc 1.15.0 or higher
- golang 1.10 or higher (Mandatory dependency for any type of build)

>**Note:** Older versions could be supported as well but not officially tested.


### For Ubuntu 19.10 or higher

Install dependencies:

```bash
sudo apt-get install qtdeclarative5-private-dev qtbase5-private-dev protobuf-compiler libprotoc-dev protobuf-compiler-grpc libgrpc++-dev libgrpc-dev libgtest-dev
```


### All-in-one build

If required versions of libraries are not found in your system, you may use all-in-one build procedure for prerequesties.

Update submodules to fetch 3rdparty dependencies:

```bash
git submodule update --init --recursive
```


## Build
```bash
mkdir build
cd build
cmake .. [-DCMAKE_PREFIX_PATH="<path/to/qt/installation>/Qt<qt_version>/<qt_version>/gcc_64/lib/cmake"]
cmake --build . [--config <RELEASE|DEBUG>] -- -j<N>
```

## Packaging
QtProtobuf has packaging support based on CPack.

### .deb
You can create .deb package for debian-like operating systems, using commands below:

```bash
mkdir build
cd build
cmake .. -DQT_PROTOBUF_MAKE_TESTS=FALSE -DQT_PROTOBUF_MAKE_EXAMPLES=FALSE
cpack -G DEB ..
```

>**Note:** Only tested on Ubuntu 19.10

### .rpm
You can create .rpm package for rpm-based operating systems, using commands below:

```bash
mkdir build
cd build
cmake .. -DQT_PROTOBUF_MAKE_TESTS=FALSE -DQT_PROTOBUF_MAKE_EXAMPLES=FALSE
cpack -G RPM ..
```

>**Note:** Only tested on OpenSUSE 15.2

# Windows Build
## Prerequesties
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

## Build
Open Qt MSVC command line and follow steps:

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" <x86|x64>
cd <directory with qtprotobuf project>
mkdir build
cd build
cmake ..
cmake --build . [--config <RELEASE|DEBUG>] -- /m:<N>
```

# Usage
## Direct usage of generator

```bash
[QT_PROTOBUF_OPTIONS="[SINGLE|MULTI]:QML:COMMENTS:FOLDER"] protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin/>qtprotobufgen --qtprotobuf_out=<output_dir> [-I/extra/proto/include/path] <protofile>.proto
```

### QT_PROTOBUF_OPTIONS

For protoc command you also may specify extra options using QT_PROTOBUF_OPTIONS environment variable and colon-separated format:

``` bash
[QT_PROTOBUF_OPTIONS="[SINGLE|MULTI]:QML:COMMENTS:FOLDER"] protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin/>qtprotobufgen --qtprotobuf_out=<output_dir> [-I/extra/proto/include/path] <protofile>.proto
```

Following options are supported:

*SINGLE* - enables single-file generation when for each *.proto* file single pair of *.h* *.cpp* files is generated

>**Note:** Enabled by default. Excluded by SINGLE

*MULTI* - enables multi-file generation when for each message separate pair of *.h* *.cpp*

>**Note:** Has higher priority than SINGLE

*QML* - enables QML code generation in protobuf classes. If is set QML-related code for lists and QML registration to be generated.

*COMMENTS* - enables comments copying from .proto files

*FOLDER* - enables folder-based generation

## Integration with CMake project

You can integrate QtProtobuf as submodule in your project or as installed in system package. Add following line in your project CMakeLists.txt:

```cmake
...
find_package(QtProtobufProject CONFIG REQUIRED COMPONENTS QtProtobuf QtGrpc)
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

*MULTI* - Enables multi-files generation mode. If provided in parameter list generator will create pair of header/source files for each message

>**Note:** multi-files generation mode is defined as deprecated by QtProtobuf team, and might have poor support in future

*QML* - Enables QML code generation in protobuf classes. If provided in parameter list QML related code for lists and QML registration to be generated.

*COMMENTS* - Enables comments copying from .proto files. If provided in parameter list message and field related comments will be copied to generated header files.

*FOLDER* - Enables folder based generation. If provided in parameter list generator will place generated artifacts to folder structure according to package of corresponding .proto file

>**Note:** enabled by default if MULTI option provided

#### qtprotobuf_link_target

qtprotobuf_link_target is cmake helper function that links generated protobuf target to your binary. It's useful when you try to link generated target to shared library or/and to executable that doesn't utilize all protobuf generated classes directly from C++ code, but requires them from QML.

***Parameters:***

*TARGET* - name of target to link to

*GENERATED_TARGET* - protobuf generated target name

#### Usefull definitions

*QT_PROTOBUF_MAKE_COVERAGE* - if **TRUE/ON** for QtProtobuf project build, QtProtobuf will be built with gcov intergration, to collect code coverage reports(usefull for developers). **FALSE** by default

*QT_PROTOBUF_MAKE_TESTS* - if **TRUE/ON** for QtProtobuf project build, tests for QtProtobuf will be built. **TRUE** by default

*QT_PROTOBUF_MAKE_EXAMPLES* - if **TRUE/ON** for QtProtobuf project build, built-in examples will be built. **TRUE** by default

*QT_PROTOBUF_STATIC* - if **TRUE/ON** for QtProtobuf project build, static libraries will be produced for all QtProtobuf targets except qml plugin. **FALSE** by default

>**Note:** In case if you use static QtProtobuf not with cmake/qmake build system, you additionaly **need manually** add QT_PROTOBUF_STATIC compiler definition

</br>
*QT_PROTOBUF_EXECUTABLE* - contains full path to QtProtobuf generator add_executable

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
