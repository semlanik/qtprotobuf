# qtprotobuf

Protobuf plugin to generate Qt classes

## Linux Build
```bash
mkdir build
cd build
cmake [-DCMAKE_PREFIX_PATH=<path/to/Qt5Config.cmake|path/to/qt5-config.cmake>] ..
make -j<N>
```

## Windows Build
```bash
mkdir build
cd build
cmake ..
cmake --build . --target qtprotobuf --config RELEASE
```

## Usage
```bash
protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin>/qtprotobuf --qtprotobuf_out=<output_dir> <protofile>.proto [--qtprotobuf_opt=out=<output_dir>]
```
