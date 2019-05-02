protoc.exe comm.proto --csharp_out=.
ren Comm.cs CommProto.cs

protoc.exe cs.proto --csharp_out=.
ren Cs.cs Messages.cs