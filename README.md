The following is an attempt at creating trading platform from scratching using C++.

The networking programming is done using the Asio C++ framework.

The files attached include a general server header file to initialize server objects.

The file messageObject is also a header file to initialize message objects.

The C++ file ClientInput.cpp is the main file for starting up the server, establishing a client connection and sending BUY, SELL, CONVERT requests
to the orderbook hosted on the server. This file also has C++ trading algorithms built within it as the file parses and aggregates important
information from the server such as: Stock selling at highest price, lowest price and order sizes. 

Another file included is websiteImplementation.cpp. This is a file used to test a client/server connection to enable a client to join through
a web address.

Custom memory management contains methods for a custom memory pool (Arena) existing within stack and a custom memory allocator that allocates memory for small size containers on stack instead of heap, to improve performance.

The code still has faults, a joint server queue needs to be implemented so that multiple message requests from clients are funnelled into 
1 server queue. 
