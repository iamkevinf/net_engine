g++ -c NetEngine/Src/tcp_server.cpp -o build/tcp_server.o
g++ -c NetEngine/Src/client_socket.cpp -o build/client_socket.o
g++ -c NetEngine/Src/cell.cpp -o build/cell.o
g++ -c NetEngine/Src/cell_task.cpp -o build/cell_task.o
g++ -c NetEngine/Src/net_msg_task.cpp -o build/net_msg_task.o

g++ -c NetEngine/Src/tcp_client.cpp -o build/tcp_client.o

g++ -c NetEngine/Src/my_server.cpp -o build/my_server.o
g++ -c NetEngine/Src/net_time.cpp -o build/net_time.o

g++ -std=c++11 -pthread NetEngine/EasyTcpServer/server.cpp -o bin/server -I NetEngine/Src build/my_server.o build/tcp_server.o build/client_socket.o build/net_time.o build/cell.o build/net_msg_task.o build/cell_task.o
g++ -std=c++11 -pthread NetEngine/EasyTcpClient/client.cpp -o bin/client -I NetEngine/Src build/tcp_client.o build/net_time.o 
