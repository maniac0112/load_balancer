#include <iostream>

#include "serverManager.h"
#include "consistentHashing.cpp"
#include <thread>

class load_balancer: IServerEventHandler{
public: 
    load_balancer(): listener(9090,this){
        //start the listener on a separate thread 
        std::thread listener_thread(&server_listener::listener, &listener);
        listener_thread.detach();
    }

    void registerServer(const server& server_object){
        server_list.push_back(server_object);
        int hash = server_hash()(server_object);
        ring.addNode(&server_list.back());
        auto it = server_list.end();
        it--;
        server_map[hash] = it; 
    }

    void removeServer(const server& server_object){
        //Remove from the Hash Ring 
        int hash = server_hash()(server_object);
        if (server_map.find(hash) == server_map.end()) return ;
        auto it = server_map[hash];
        ring.removeNode(&*it);
        server_list.erase(it);
    }

    void ping(const server& server_object){
        int hash = server_hash()(server_object);
        if (server_map.find(hash) == server_map.end()) return ;
        auto it = server_map[hash];
        it->last_visit = std::chrono::steady_clock::now();
    }

private: 
    std::list<server> server_list;
    std::unordered_map<int, std::list<server>::iterator> server_map;
    server_listener listener; 
    consistentHashRing ring; 
};

int main(){
    std::cout<<"Spnning up the Load Balancer \n";
    //It will have two threads, one will accept the client request and another will be listening for registrations! 


}