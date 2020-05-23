export PATH=$PATH:$PWD/bin
export GOBIN=$PWD/bin
export RPC_PATH=$PWD/../
export GO111MODULE=on

go get -u github.com/golang/protobuf/protoc-gen-go@v1.3.3

mkdir -p qtprotobuf_tutorial
rm -f qtprotobuf_tutorial/*.pb.go
protoc -I../ --go_out=plugins=grpc:qtprotobuf_tutorial ../tutorial.proto

go build

./tutorialserver
