#include <map>
#include "serverManager.h"
#include <list> 

class consistentHashRing{
public:
    consistentHashRing() = default; 

    void addNode(server* new_server){
        int hash = getHash(&(new_server->addr));
        HashRing[hash] = new_server;
    }

    void removeNode(server* new_server){
        int hash = getHash(&(new_server->addr));
        //our map should have this! 
        HashRing.erase(hash);
    }

    server* getNode(sockaddr_in* client){
        int hash = getHash(client);
        //check for this hash in the hash ring 
        auto it = HashRing.lower_bound(hash);

        if (it == HashRing.end()){
            it = HashRing.begin();
        }

        return it->second; 
    }

private: 

    size_t getHash(sockaddr_in* input){
        size_t h1 = std::hash<uint32_t>()(input->sin_addr.s_addr);
        size_t h2 = std::hash<uint16_t>()(input->sin_port);
        return h1 ^ (h2 << 1);
    }

    std::map<size_t,server*> HashRing; 
};