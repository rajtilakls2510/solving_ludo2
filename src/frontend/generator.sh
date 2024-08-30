for proto_file in ../proto/*.proto; do
  /usr/local/bin/bin/protoc -I=../proto $proto_file \
  --js_out=import_style=commonjs,binary:./src/generated \
  --grpc-web_out=import_style=commonjs,mode=grpcwebtext:./src/generated
done

