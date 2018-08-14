@echo off

SET ProtocPath=.\protoc.exe
SET ProtocCSPath=.\ProtoGen\protogen.exe
SET ProtoPath=.\proto
SET CPPOutPath=..\LockStepDemo\Proto
SET CSOutPath=.\proto_cs

SET FilePath=.\proto

for /f "delims=" %%i in ('dir /b "%FilePath%\*.proto"') do (
	%ProtocCSPath% -i:proto/%%i -o:proto_cs/%%~ni.cs
	%ProtocPath% --proto_path=%ProtoPath% --cpp_out=%CPPOutPath% %%~ni.proto
)

pause
