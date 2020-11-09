package main

import (
	"context"
	"log"
	"net"

	"tutorialserver/qtprotobuf_tutorial"

	grpc "google.golang.org/grpc"
)

type echoServer struct{}

func (s *echoServer) Echo(ctx context.Context, req *qtprotobuf_tutorial.EchoRequest) (*qtprotobuf_tutorial.EchoResponse, error) {
	return &qtprotobuf_tutorial.EchoResponse{Message: req.Message}, nil
}

func main() {
	lis, err := net.Listen("tcp", ":65000")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	grpcServer := grpc.NewServer()
	qtprotobuf_tutorial.RegisterEchoServiceServer(grpcServer, &echoServer{})
	grpcServer.Serve(lis)
}
