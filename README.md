# MPRPC - Distributed Lightweight RPC Framework

A high-performance RPC framework implemented in modern C++11, based on the Muduo network library. It uses Protobuf for message serialization and Zookeeper for service discovery.

---

## 🌐 Features

- 🔌 **Protocol Buffers** for service interface definition and serialization
- 🧠 **Custom RPC Core**: includes `RpcProvider`, `RpcChannel`, `RpcController`, and `MprpcApplication`
- 🧵 **Asynchronous, Event-driven Architecture** powered by Muduo (Reactor model)
- 🗂️ **Service Discovery and Registration** using Zookeeper (via native C binding)
- 🧱 Modular design, support for client/server separation and dynamic service scaling
- 🛠️ Build system based on CMake for ease of use and cross-platform compatibility

---

## 📁 Project Structure

mprpc/
├── bin/ # Compiled binaries and logs
├── build/ # CMake build directory
├── example/ # Sample services and clients (caller/callee)
├── lib/ # Static library output and headers
├── src/ # Core framework source code
│ ├── mprpcapplication.cc / .h
│ ├── mprpcchannel.cc / .h
│ ├── mprpccontroller.cc / .h
│ ├── rpcprovider.cc / .h
│ ├── zookeeperutil.cc / .h
│ └── logger.cc / .h, lockqueue.h
├── test/ # Protobuf test case
├── CMakeLists.txt # Project entry
└── README.md

## 🚀 Quick Start

### 🔧 1. Dependencies

- C++11
- [Muduo]
- [Protobuf]
- [Zookeeper C Client]

### 🧱 2. Build

```bash
# Inside project root
mkdir build && cd build
cmake ..
make -j4
