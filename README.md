# ConcurrentEngine_Light

A lightweight, modular and extensible C++ concurrency task scheduling engine with multi-strategy support.

This project aims to provide a customizable and efficient concurrency framework with multiple schedulers (FIFO, Priority, DAG), task encapsulation using `std::future`, and optional rejection policies for overload handling.

---

## Features

- **ThreadPool with dynamic worker management**
- **Multiple schedulers**:
  - FIFO (First-In-First-Out)
  - Priority Queue
  - DAG-based dependency scheduler
- **Generic task encapsulation** using `std::packaged_task` / `std::function`
- **Task rejection strategies**: BLOCK, DISCARD, THROW
- **mini_Redis example**: demonstrating concurrent task execution
- **Thread-safe logger**
- Modern C++ (C++20/23) design

---

## Build & Run

### Requirements

- C++20 or above
- MinGW / GCC / Clang
- CMake 3.14+ (optional if using batch scripts)

## Build Using CMake
```bash
# Clone project
git clone https://github.com/Gavin-Dementia/ConcurrentEngine_Light.git
cd ConcurrentEngine_Light/ConcurrentEngine

# Build
mkdir build && cd build
cmake ..
make

# Run main example
./main
```

## Build & Run mini-Redis Example
```bash
cd examples/mini_redis

# Use the batch script for Windows
build_mini_redis.bat

# Run interactive mini-Redis CLI
test.exe
```
- Commands supported: GET, SET, DEL, QUIT
- All commands are executed concurrently using the ThreadPool
- The scheduler (FIFO, Priority, or DAG) can be changed in test.cpp


# Project Structure 
```bash
ConcurrentEngine_Light/
├── include/             # Public headers
│   └── threadPool/...
├── src/                 # Core implementations
│   ├── core/
│   ├── logger/
│   └── scheduler/
├── examples/            # Usage demos (e.g. mini_redis)
├── docs/ARCHITECTURE.md # System architecture
├── CMakeLists.txt
└── README.md // YOUR ARE HERE
```

# Example Task Example(Dependency Scheduling)
## Basic Task Submission
```bash
#include <threadPool/threadPool.hpp>
using namespace ConcurrentEngine;

int main() {
    ThreadPool pool(4); // 4 worker threads

    // Submit simple tasks
    auto f1 = pool.submit([] { return 100; });
    auto f2 = pool.submit([] { return 200; });
    std::cout << "Sum: " << f1.get() + f2.get() << "\n";

    return 0;
}
```
## Priority Scheduling 
```bash 
pool.setScheduler(std::make_unique<Scheduler::PriorityScheduler>());
pool.submit([] { std::cout << "High priority\n"; }, /*priority=*/10);
```

## DAG Scheduling
```bash
DAGScheduler dag;
dag.addTask("A", [] { std::cout << "Task A\n"; });
dag.addTask("B", [] { std::cout << "Task B\n"; });
dag.addTask("C", [] { std::cout << "Task C\n"; });

// C depends on A and B
dag.addDependency("A", "C");
dag.addDependency("B", "C");

// Run tasks
dag.run();
```

# Execution Order (sample output)
```bash
Task A
Task B
Task C
```

# Modules Overview
```bash
| Module                | Description                              |
| --------------------- | ---------------------------------------- |
| **ThreadPool**        | Manages worker threads & task submission |
| **Scheduler**         | Abstract interface for task dispatching  |
| **FIFOScheduler**     | Queue-based scheduler (default)          |
| **PriorityScheduler** | Priority queue scheduler                 |
| **DAGScheduler**      | Dependency-aware task execution          |
| **Logger**            | Thread-safe logging with timestamps      |
```
## Example: Mini-Redis
This repo includes a concurrent mini-Redis implementation in examples/mini_redis/,
powered by the ThreadPool + schedulers.
### Run:
```bash
cd examples/mini_redis
./build_mini_redis.bat
```
### Example session:
```bash
mini-Redis CLI (type QUIT to exit)
> SET key hello
OK
> GET key
hello
> PING
PONG
```

# License

This project is licensed under the MIT License. See LICENSE for details.
