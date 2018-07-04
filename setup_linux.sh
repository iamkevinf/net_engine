g++ -c NetEngine/Src/tcp_server.cpp -o build/tcp_server.o
g++ -c NetEngine/Src/client_socket.cpp -o build/client_socket.o

g++ -c NetEngine/Src/tcp_client.cpp -o build/tcp_client.o

g++ -std=c++11 -pthread NetEngine/EasyTcpServer/server.cpp -o bin/server -I NetEngine/Src build/tcp_server.o build/client_socket.o
g++ -std=c++11 -pthread NetEngine/EasyTcpClient/client.cpp -o bin/client -I NetEngine/Src build/tcp_client.o
