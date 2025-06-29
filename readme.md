# Simple UDP Load Balancer with Consistent Hashing

This project demonstrates a basic, in-memory UDP-based load balancer where servers self-register via UDP, send periodic pings to stay active, and client requests are routed using consistent hashing for session affinity.

## Features

* **Server Registration & Keep-Alive**: Servers send `REGISTER` and `PING` messages to the listener on a well-known port. The load balancer tracks each server's last-seen timestamp in memory.

* **Consistent Hashing**: Client requests (UDP or TCP) are mapped to backend servers using a consistent hash ring built on each server's IP and port. This ensures minimal remapping when servers join or leave.

* **In-Memory State**: All server metadata is stored in process memory, avoiding external dependencies (no database).

* **Stateless Forwarding**: Client requests are forwarded statelessly to chosen servers, making the balancer simple and easy to extend.

## Repository Structure

```text
./
├── consistentHashing.cpp   # Implementation of the consistent hash ring
├── serverManager.h/.cpp    # ServerRegistry and LoadBalancer logic (add, remove, ping)
├── load-balancer.cpp       # Main entry: UDP listener hooking into LoadBalancer
├── readme.md               # (This file) Project overview & build instructions
└── CMakeLists.txt          # (Optional) For building with CMake
```

## Build Instructions

```bash
# Using g++ directly:
g++ -std=c++17 -pthread load-balancer.cpp serverManager.cpp consistentHashing.cpp -o udp_load_balancer

# Or with CMake:
mkdir build && cd build
cmake ..
make
```

## Future Improvements

- **TTL-Based Eviction**: Remove servers automatically if no `PING` received within a configurable timeout.
- **Virtual Nodes**: Improve key distribution by adding multiple replicas per server in the hash ring.
- **Thread Pool**: Handle incoming packets and forwarding concurrently with worker threads.
- **Configuration**: Externalize settings (ports, TTL, number of replicas) via a config file or flags.

## License

This project is provided "as is" for educational purposes. Feel free to use and modify it in your own experiments.

```
