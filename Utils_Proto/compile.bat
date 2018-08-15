@echo off

SET ProtocPath=.\protoc.exe
SET ProtocCSPath=.\ProtoGen\protogen.exe
SET ProtoPath=.\proto
SET CPPOutPath=..\NetEngine\LockStepDemo\Proto
SET CSOutPath=..\UnityLockStepDemo\Assets\proto_cs

SET FilePath=.\proto

for /f "delims=" %%i in ('dir /b "%FilePath%\*.proto"') do (
	%ProtocCSPath% -i:%ProtoPath%/%%i -o:%CSOutPath%/%%~ni.cs
	%ProtocPath% --proto_path=%ProtoPath% --cpp_out=%CPPOutPath% %%~ni.proto
)

pause
