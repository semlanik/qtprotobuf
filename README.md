# qtprotobuf

Protobuf plugin to generate Qt classes

# Build

```bash
mkdir build
cd build
cmake ..
make -j<N>
```

# Usage

```bash
protoc --plugin=protoc-gen-qtprotobuf=<path/to/bin>/qtprotobuf --qtprotobuf_out=<output_dir> <protofile>.proto
```
