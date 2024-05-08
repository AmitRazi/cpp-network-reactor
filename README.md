# Reactor Model Implementation in C++

## Overview
This repository contains an implementation of the reactor design pattern in C++ for handling simultaneous service requests using non-blocking I/O. It demonstrates efficient network communication via sockets to manage multiple client connections.

## Features
- Non-blocking I/O with socket operations
- Management of multiple client connections
- Utilization of the poll system call for I/O channel management
- Configurable handler functions for I/O operations
- Proper shutdown and error handling mechanisms

## Requirements
- C++ compiler (GCC recommended)
- Linux OS

## Compilation Instructions
Compile the project using the following command:
```bash
g++ -o reactor main.cpp -pthread
```
Ensure that the source files are correctly specified and that the pthread library is linked.

## Execution
Run the application with:
```bash
./reactor
```
