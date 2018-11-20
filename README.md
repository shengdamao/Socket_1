
1.0 利用 socket, bind, listen, accept, read, write, close (srv) 完成回射 cli/srv 模型
1.1 利用 fork(), 完成peer_To_Peer聊天程序， 子进程写，父进程读。
1.2 利用 fork(), 完成回射客户端程序的并发服务器，子进程负责通讯细节（回射客户端），父进程负责监听 新连接。
1.3 利用 封装自定义函数 readn writen readline 函数解决粘包问题。
2.0 利用 select(), 完成 监听I/O 和 读写I/O 的同时处理。 fork()使父进程监听，新的子进程负责每个新连接的通讯细节
2.1 单进程 利用 select 完成并发服务器。
2.2 
