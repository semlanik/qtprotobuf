# QtProtobuf

Protobuf plugin to generate Qt classes

## Linux Build
### Prerequesties
Check installation of protobuf and grpc in your system. Minimum required versions are protobuf-3.6.0 and grpc-1.15.0.

**Note:** Older versions could be supported as well but not officially tested.

#### All-in-one build
If required versions of libraries are not found in your system, you may use all-in-one build procedure for prerequesties

Update submodules to fetch 3rdparty dependencies:

```bash
git submodule update --init --recursive
```
### Build
```bash
mkdir build
cd build
cmake ..
cmake --build . [--config <RELEASE|DEBUG>] -- -j<N>
```

## Windows Build
### Prerequesties

Download and install:

- Qt 5.12.3 or higher [1](https://download.qt.io/official_releases/qt/)
- cmake-3.1 or higher [2](https://cmake.org/download/)
- Strawberry perl 5.28 [3](http://strawberryperl.com/)
- GoLang 1.10 or higher [4](https://golang.org/dl/)
- Yasm 1.3 or higher [5](http://yasm.tortall.net/Download.html)
- Actual Visual Studio Compiler with cmake support that required by Qt [6](https://visualstudio.microsoft.com/downloads/)

**Note:** All applications should be in PATH

Update submodules to fetch 3rdparty dependencies:

```bash
git submodule update --init --recursive
```

### Build
Open Qt MSVC command line and follow steps:

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
cd <directory with qtprotobuf project>
mkdir build
cd build
cmake ..
cmake --build . [--config <RELEASE|DEBUG>] -- /m:<N>
```
**Note:** 64-bit build is not supported yet

## Usage
```bash
protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin>/qtprotobufgen --qtprotobuf_out=<output_dir> <protofile>.proto [--qtprotobuf_opt=out=<output_dir>]
```
## Running tests
```bash
cd <build directory>
ctest
```
## Documentation generation

Project uses doxygen for documentation generation.

#### For Windows additionally install:
* [doxygen](http://www.doxygen.nl/download.html)
* [graphviz](https://graphviz.gitlab.io/_pages/Download/Download_windows.html)


You can generate documentation:

```bash
mkdir build
cd build
cmake ..
cmake --build . --target doc
```
