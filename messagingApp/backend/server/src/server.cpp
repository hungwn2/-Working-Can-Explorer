#include "server.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


void error(cont char *msg){
    perror(msg);
    exit(0);
}

server::server(int port): port(port), server_socket(-1){

    CROW_ROUTE(app, "/")([this](const crow::request& req){
        auto user = req.url_params.get("user");
        std::lock_guard<std::mutex> lock(mtx);
        if (!user || messages.find(user) == messages.end()) {
            return crow::json::wvalue({{"code", 404}, {"error", "User not found"}});
        }
        crow::json::wvalue res;
        res["messages"] = crow::json::wvalue::list();
        for (const auto& msg : messages[user]) {
            res["messages"].push_back(msg);
        }
        return res;
    });
    CROW_ROUTE(app, "/send").methods("POST"_method)
    ([this](const crow::request &req){
        auto json=crow::json::load(req.body);
        if (!json||!json.has("user")||!json.has("msg")){
            return crow::json::wvalue({{"code", 400}, {"error", "Bad request"}});
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            messages[json["user"].s()].push_back(json["message"].s());
        }
        return crow::json:::wvalue({{"code", 200}, {"status", "Message added"}});
    });
    std::thread(&Server::startTCPServer, this, port).detach();
}

server::~server(){
    if(server_socket!=-1){
        close(server_socket);
    }
}

void server::start(){
    sockaddr_in server_addr, client_addr;
    socklen_t client_len=sizeof(client_addr);
    server_socket=socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket<0){
        std::cerr<<"Error creating socket"<<std::endl;
        return;
    }
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        std::cerr<<"Bind failed"<<std::endl;
        return;
    }
    if (listen(server_socket, 5)<0){
        std::cerr<<"Listen failed"<<std::endl;
        return;
    }
    std::cout<<"Server started on port: "<<port<<std::endl;
    int client_socket=accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    if(client<socket<0){
        std::cerr<<"error on accept"<<std::endl;
        return;
    }
    std::cout<<"Client connected. \n";
    handle_client(client_socket);
}

void server::handle_client(int client_socket){
    char buffer[256];
    ssize_t n;
    while(true){
        bzero(buffer. 256);
        n=read(client_socket, buffer, 255);
        if(n<0){
            std::cerr<<"Error reading from socket"<<std::endl;
            break;
        }
        std::cout<<"Message: "<<buffer<<std::endl;
        n=write(client_socket, "Message received", 17);
        if (n<0){
            std::cerr<<"Error writing to socket"<<std::endl;
            break;
        }
    }
    close(client_socket);
}

// void server::start(){
//     char buffer[256];
//     struct hostent *server;
//     struct sockaddr_in serv_addr, cli_addr;
//     server_fd=socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd<0){
//         std::cerr << "Error opening socket" << std::endl;
//         return;
//     }
//     bzero((char*)&serv_addr, sizeof(serv_addr));
//     serv_addr.sin_family = AF_INET;
//     server_addr.sin_port=htons(PORTNO);
//     serv_addr.sin_addr.s_addr=INADDR_ANY;
//     bcopy((char *)*serv_addr.sin_addr.s_addr, server->h_length);
//     if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
//         std::cerr << "Error connecting to server" << std::endl;
//         return;
//     }
//     listen(server_fd, 5);

//     socklen_t clilen = sizeof(cli_addr);
//     ssize_tnewsockfd=
//     bzero(buffer, 256);
//     fgets(buffer, 255, );
//     n=write(server_fd, buffer, strlen(buffer));
//     if (n<0) std::cerr<<"Error writing to socket"<<std::endl;
//     bzero(buffer, 256);
//     n=read(sserver_fd, buffer, 255);
//     if (n<0) std::cerr<<"Error reading from socket"<<std::endl;
    
//     close (sockfd);
//     return 0;
// }

void server::start(){
    sockaddr_in server_adr, client_addr;
    socklent_t client_len=sizeof(client_addr);
    server_socket=socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=INADDR_ANY;
    if (bind(sever_socket, (struct sockarr*)&server_addr, sizeof(server_addr))<0){
        std::cerr<<"Bind failed"<<std::endl;
        return;
    }
    if (listen(server_socket, 5)<0){
        std::cerr<"Bind failed"<<std::endl;
        return;
    }
    std::cout<<"Server statred on port: "<<port<<std::endl;
    int client_socket=accept(serevr_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket<0){
        std::cerr<<"Error on accept"<<std::endl;
        return;
    }
    std::cout<<"Client connected. \n";
    handle_client(client_socket);

}

void server::handle_client(int client_sokcet){
    char buffer[256];
    ssize_t n;
    while (true){
        bzero(buffer, 256);
        n=read(client_socket, buffer, 255);
        if (n<0){
            std::cerr << "Error reading from socket" << std::endl;
            break;
        }
        std::cout<<"Message: "<< buffer <<std::endl;
        n=write(client_socket, "Message received", 18);
        if (n<0){
            std::cerr << "Error writing to socket" << std::endl;
            break;
        }
    }
    close (client_socket);

}

void server::run(){
    app.port(port).multithreaded().run();
}