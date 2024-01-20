**CPP Trading Platform:**
==============================

![CPP Trading Platform](trading_plat_logo.png)

This repository is an endeavor to build a trading platform from scratch using C++. The networking programming is achieved through the Asio C++ framework.

Files Overview:
server.h

Path: src/server.h
Initializes server objects for the trading platform.
messageObject.h

Path: src/messageObject.h
Initializes message objects for communication.
ClientInput.cpp

Path: src/ClientInput.cpp
Main file for starting the server, establishing client connections, and handling BUY, SELL, CONVERT requests. Incorporates C++ trading algorithms, parsing essential information from the server (e.g., highest/lowest stock prices and order sizes).
websiteImplementation.cpp

Path: src/websiteImplementation.cpp
A file designed for testing client/server connection through a web address.
customMemoryManagement.cpp

Path: src/customMemoryManagement.cpp
Implements custom memory management methods, including a custom memory pool (Arena) within the stack and a memory allocator that allocates memory for small-size containers on the stack to enhance performance.

Known Improvements Needed:
The current code has a flaw: a joint server queue needs implementation to consolidate multiple message requests from clients into a single server queue for improved efficiency.
