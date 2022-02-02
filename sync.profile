%modules = ( # path to module name map
    "QtProtobuf" => "$basedir/src/protobuf",
    "QtProtobufWellKnownTypes" => "$basedir/src/wellknowntypes",
    "QtProtobufQtTypes" => "$basedir/src/qttypes",
    "QtProtobufQuick" => "$basedir/src/protobufquick",
    "QtGrpc" => "$basedir/src/grpc",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);

@ignore_headers = ( );
