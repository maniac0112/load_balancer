#pragma once 

#include <chrono>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <iostream>

struct server{
    sockaddr_in addr; //Will have Address and Port of the server 
    std::chrono::time_point<std::chrono::steady_clock> last_visit; 
};

struct server_hash {
    size_t operator()(const server& s) const {
        // Normalize IP and port to host byte order
        uint32_t ip_host = ntohl(s.addr.sin_addr.s_addr);
        uint16_t port_host = ntohs(s.addr.sin_port);

        size_t h1 = std::hash<uint32_t>()(ip_host);
        size_t h2 = std::hash<uint16_t>()(port_host);

        return h1 ^ (h2 << 1); // Simple bit mixing
    }
};

class IServerEventHandler{
public: 
    virtual void registerServer(const server& s) = 0;
    virtual void ping(sockaddr_in addr) = 0; 
    virtual ~IServerEventHandler() = default;
};


class server_listener{   
public: 
    server_listener(int port, IServerEventHandler* ptr);
    server_listener* getServerManagerInstance();
    void listener(); //Process a register request and if valid, passes the server structure to load balancer or routes ping request to load balancer 
private:
    IServerEventHandler* _load_balancer; 
    int _port; 
};
