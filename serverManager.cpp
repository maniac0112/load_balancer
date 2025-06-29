#pragma once 

#include "serverManager.h"

const int SERVER_QUEUE_LIMIT = 100; 

server_listener::server_listener(int port, IServerEventHandler* ptr){
    _load_balancer = ptr; 
    _port = port; 
}

void server_listener::listener(){
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(_port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    socklen_t server_addr_size = sizeof(server_address); 

    char read_buffer[1024];
    std::string ack_message = "Server Added to Load Balancer"; 

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    bind(server_socket, reinterpret_cast<const sockaddr*>(&server_address), server_addr_size); 

    //start accepting the requests
    while(1){
        sockaddr_in client_addr; 
        socklen_t client_addr_size = sizeof(client_addr); 
        int read_bytes = recvfrom(server_socket,read_buffer, sizeof(read_buffer), 0, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_size); 
        if (read_bytes < 0) {
            perror("recvfrom failed");
            return ;
        }

        read_buffer[read_bytes] = '\0';
        //Since this is a datagram packet, we'll send a datagram ack on success to indicate the status!
        if (strcmp(read_buffer, "REGISTER") == 0){

            server server_object{client_addr, std::chrono::steady_clock::now()};
            
            _load_balancer->registerServer(server_object);
            //request succeeded. We only send response for this 

            int client_socket = socket(AF_INET, SOCK_DGRAM, 0);

            if (client_socket < 0) {
                perror("socket creation failed");
                continue;
            }

            sendto(client_socket, ack_message.data(), ack_message.size(), 0, reinterpret_cast<const sockaddr*>(&client_addr), client_addr_size);
            close(client_socket);

        }else if (strcmp(read_buffer, "PING") == 0){
            _load_balancer->ping(client_addr); 
            //We send to response to server
            //Even Listener is unaware when Ping request prevented the server to stay valid! 
        }else{
            std::cout<<"Invalid Client Request \n"; 
        }
    }

    close(server_socket);

} //Process a register request and if valid, passes the server structure load balancer 

