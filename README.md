# qtprotobuf

Protobuf plugin to generate Qt classes

# Linux Build

```bash
mkdir build
cd build
cmake ..
make -j<N>
```

# Windows Build

```Windows Qt command line
mkdir build
cd build
cmake ..
cmake --build . --target qtprotobuf --config RELEASE
```

# Usage

```bash
protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin>/qtprotobuf --qtprotobuf_out=<output_dir> <protofile>.proto
```
