//Networking headers for connecting to server
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <messageObject.h>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <serverObject.h>

//C++ headers used in the scalping trading bot
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

class Configuration {

public:
  string client_name;
  string server_hostname;
  int server_port;
  
  //Constructor for connection between client and server
  //test_mode boolean determines whether we're sending requests to server or not
  Configuration(bool test_mode) : client_name("Gavin")){
    exchange_port = 20000; /* Default text based port */
    if(true == test_mode) {
      server_hostname = "Exchange: " + client;
      server_port += 2;
    } else {
      server_hostname = "offline";
    }
  }
};

enum class Direction: uint32_t { 
  Buy, 
  Sell
};

string direction_to_string(Direction dir) {
  switch (dir) {
  case Direction::Buy : return ("BUY"); break;
  case Direction::Sell : return ("SELL"); break;
  }
}

Direction string_to_direction(string str) {
  if (str == "BUY") {
    return Direction::Buy;
  }
  return Direction::Sell;
}

/* Connection establishes a read/write connection to the exchange,
   and facilitates communication with it */

//Connection establishes a read/write connection to the exchange, and enables communication with it
bool Connect(const string& host, const uint16_t port)
	{

    //Use asio to get endpoints of connection between client and server
    asio::ip::tcp::resolver resolver(m_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, to_string(port));

    //Establish connection between client and server
    m_connection = make_unique<connection<T>>(connection<T>::owner::client_name, m_context, asio::ip::tcp::socket(m_context), m_qMessagesIn);
    
    //Send address as arguments to connect to server
    m_connection->ConnectToServer(endpoints);
  }

  if (exception& e) //if exception is detected then:
  {
    cerr << "[INFO] Client Exception: " << e.what() << "\n";
    return false;
  }
  return true;
}

  //After establishing connection with server, send string:
void send_server(string input) {
  string line(input);
  //Make the input string all uppercase:
  transform(line.begin(), line.end(), line.begin(), ::toupper);
  int res = fprintf(this->out, "%s\n", line.c_str());
  if (res < 0) {
    runtime_error("error sending to server");
  }
}

//Clients all send hello once connected to server/exchange
void send_hello_server(string client_name) {
  vector<string> data;
  data.push_back(string("HELLO"));
  data.push_back(client_name);
  send_server(*data.end());
}

//Adding orderbook functionalities to client messages:
//ADD an order:

void order_add(int order_id, string symbol, Direction dir, int price, int size) {
  vector<string> data;
  data.push_back("ADD");
  data.push_back(to_string(order_id));
  data.push_back(symbol);
  data.push_back(direction_to_string(dir));
  data.push_back(to_string(price));
  data.push_back(to_string(size));
  client::message<Direction> msg;
  msg.header.id = Direction::ServerPing;
  msg.Send(data);
}

//CONVERT a bond type:
  void order_bond(int order_id, string symbol, Direction dir, int size) {
    vector<string> data;
    data.push_back("CONVERT");
    data.push_back(to_string(order_id));
    data.push_back(symbol);
    data.push_back(direction_to_string(dir));
    data.push_back(to_string(size));
    client::message<Direction> msg;
		msg.header.id = Direction::ServerPing;
    msg.Send(data);
  }

//CANCEL order:
  void order_cancel(int order_id) {
    vector<string> data;
    data.push_back(string("CANCEL"));
    data.push_back(to_string(order_id));
    client::message<Direction> msg;
    msg.header.id = Direction::ServerPing;
    msg.Send(data);
  }

  //Client taking in and reading messages from server
  string read_from_server()
  {
    ///Limit message size to 10000 char
    const size_t len = 10000;
    char buf[len];
    if(asio::(buf, len, this->in)){
      runtime_error("reading line from socket");
    }
    //Create lambda func to remove lastline of message sent
    [](string result(buf)) -> {result.resize(result.length() - 1);}
    return result;
  }
};

//Possible message types received from the exchange:
//Exchange starts by giving information of all the current orders in the orderbooks consisting of position size and ticker symbol.
struct Hello {
  struct Symbol_position {
    string symbol;
    int position;
  };
  vector <Symbol_position> positions;
};

struct Error {
  string message;
};

//Struct to create object of custom type that can be initialized when receiving a new orderbook entry sent by the exchange:
struct Book {
  struct Book_entry {
    int price;
    int size;
  };
  string symbol;
  vector <Book_entry> buy_entries;
  vector <Book_entry> sell_entries;
};

//Struct to create object when receiving trade message
struct Trade {
  string symbol;
  int price;
  int size;
};

//Struct to create object when receiving trade reject message
struct Reject {
  int order_id;
  string message;
};

//Struct to create object when receiving fill order message
struct Fill {
  int order_id;
  string symbol;
  Direction dir;
  int price;
  int size;
};


//Function to parse through hello message and determine which symbol order requests are on the orderbook:
Hello parse_hello_message(vector<string> tokens) { //Argument tokens represents size of received message
  vector <Hello::Symbol_position> positions;
  for (int i = 1; i < tokens.size(); ++i) {
    if (tokens[i]==":") {
      tokens[i].erase();
    } //remove all : from the message and assign the result to symbol_pos:
    vector <string> symbol_pos = tokens;
    string symbol = symbol_pos[0];
    int position = stoi(symbol_pos[1]);
    positions.push_back((Hello::Symbol_position {symbol, position}));
  }
  return Hello {positions};
}

Error parse_error_message(vector<string> tokens) {
  tokens.erase(tokens.begin());
  return Error {tokens};
}

//In orderbook, an order contains: stock symbol, BUY/SELL and position size, this is parsed and evaluated:
Book parse_book_message(vector<string> tokens) {
  string symbol = tokens[1]; //stock symbol in order request is parsed and assigned to variable symbol.
  vector <Book::Book_entry> buy_entries;

  string buyRequest = tokens[2]; //If buyRequest is present then tokens[2] = BUY;

  int i = 3; //Parse through the rest of the message after BUY request to determine position size and price:

  while(tokens[i] != "SELL") {
    if (tokens[i]==":") {
      tokens[i].erase();
    }
    book_entry = tokens;
    int price = stoi(book_entry[0]);
    int size = stoi(book_entry[1]);
    buy_entries.push_back({price, size});

    i++;
  } //Output is the position size and price of the buy request

  //Now parsing through remainder of message to get SELL Request information if existent:
  vector <Book::Book_entry> sell_entries;
  for (int j = i + 1; j < tokens.size(); ++j) {
     if (tokens[j]==":") {
      tokens[j].erase();
    }
    book_entry = tokens;
    int price = stoi(book_entry[0]);
    int size = stoi(book_entry[1]);
    sell_entries.push_back({price, size});
  }
  return Book {symbol, buy_entries, sell_entries};
} //So full information regarding BUY/SELL request has been parsed from the orderbook request

//If trade is message type then create variable of custom type trade and fill its parameters with the information from the orderbook request:
Trade parse_trade_message(vector<string> tokens) {
  string symbol = tokens[1];
  int price = stoi(tokens[2]);
  int size = stoi(tokens[3]);
  return Trade {symbol, price, size};
}

//If reject is message type then create variable of custom type reject and fill its parameters with the information from the orderbook request:
Reject parse_reject_message(vector<string> tokens) {
  int order_id = stoi(tokens[1]);
  tokens.erase(tokens.begin());
  tokens.erase(tokens.begin());
  string message = join(" ", tokens);
  return Reject {order_id, message};
}

Fill parse_fill_message(vector<string> tokens) {
  int order_id = stoi(tokens[1]);
  string symbol = tokens[2];
  Direction dir = string_to_direction(tokens[3]);
  int price = stoi(tokens[4]);
  int size = stoi(tokens[5]);
  return Fill {order_id, symbol, dir, price, size};
}

Out parse_out_message(vector<string> tokens) {
  int order_id = stoi(tokens[1]);
  return Out {order_id};
}

//Helper functions for better understanding of server/client outputs:

ostream& operator<<(ostream&os, const Hello::Symbol_position &symbol_position) {
  os << symbol_position.symbol << ":"  << symbol_position.position;
  return os;
}

ostream& operator<<(ostream&os, const Hello &hello) {
  os << "HELLO";
  for (const Hello::Symbol_position &symbol_position : hello.positions){
    os << " " << symbol_position;
  }
  return os;
}

ostream& operator<<(ostream&os, const Open &open) {
  os << "OPEN";
  for (const string &symbol : open.symbols){
    os << " " << symbol;
  }
  return os;
}
ostream& operator<<(stream&os, const Close &close) {
  os << "CLOSE";
  for (const string &symbol : close.symbols){
    os << " " << symbol;
  }
  return os;
}
ostream& operator<<(ostream&os, const Error &error) {
  os << "ERROR " << error.message;
  return os;
}
ostream& operator<<(ostream&os, const Book::Book_entry &book_entry) {
  os << book_entry.price << ":" << book_entry.size;
  return os;
}
ostream& operator<<(ostream&os, const Book &book) {
  os << "BOOK " << book.symbol << " BUY";
  for(const Book::Book_entry &book_entry : book.buy_entries) {
    os << " " << book_entry;
  }
  os << " SELL";
  for(const Book::Book_entry &book_entry : book.sell_entries) {
    os << " " << book_entry;
  }
  return os;
}
ostream& operator<<(ostream&os, const Trade &trade) {
  os << "TRADE " << trade.symbol << " " << trade.price << " " << trade.size;
  return os;
}
ostream& operator<<(ostream&os, const Reject &reject) {
  os << "REJECT " << reject.order_id << " " << reject.message ;
  return os;
}
ostream& operator<<(ostream&os, const Fill &fill) {
  os << "FILL " << fill.order_id << " " << fill.symbol << " " << direction_to_string(fill.dir) << " " << fill.price << " " << fill.size;
  return os;
}
ostream& operator<<(ostream &os, const Out &out) {
  os << "OUT " << out.order_id;
  return os;
}

int main(int argc, char *argv[])
{
    //Set test_mode to true to send requests to server
    bool test_mode = true;
    Configuration config(test_mode);
    Connect conn("127.0.0.1", 60000);

    conn.send_hello_server(config.client_name);

    while(true) {
      string message = conn.read_from_server();
      for (auto i: message) {
        if (i==" ") {
          i.erase();
        }
      }
      vector<string> tokens = message;
      string message_type = tokens[0];

      if (message_type == "HELLO") {
        Hello hello = parse_hello_message(tokens);
      }
      else if (message_type == "ERROR") {
        Error error = parse_error_message(tokens);
      }
      else if (message_type == "BOOK") {
        Book book = parse_book_message(tokens);
      }
      else if (message_type == "TRADE") {
        Trade trade = parse_trade_message(tokens);
      }
      else if (message_type == "REJECT") {
        Reject reject = parse_reject_message(tokens);
      }
      else if (message_type == "FILL") {
        Fill fill = parse_fill_message(tokens);
      }
    }
    return 0;
}
