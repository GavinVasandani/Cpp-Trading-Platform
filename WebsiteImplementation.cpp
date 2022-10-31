//Program to evaluate incoming bytes from user, if platform is run over a website
#include <chrono>
#include <iostream>
using namespace std;


//To not have WIN32 outputted in terminal we do: 
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif


#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp> //Used to handle movement of memory
#include <asio/ts/internet.hpp> //Used to prepare asio for all internet networking


//Immediately outputting data when it arrives:

vector<char> vBuffer(1 * 1024); //Here we define a vector to store the bytes, the vector is allocated size 20*1024

//Creating a func to take received data from server:

void GrabSomeData(asio::ip::tcp::socket& socket) { //arg of func is the socket which is the conection between client and server

	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()), [&](error_code ec, size_t length) {
		if (!ec) {
			cout << "Read " << length << " bytes" << endl;
			for (auto i : vBuffer) {
				cout << i;
			}
			GrabSomeData(socket);
		}
		}
	);

}

int main() {
	
	asio::error_code ec; //here we're using asio namespace as we're trying to access classes in asio file

	asio::io_context context;

	asio::io_context::work idleWork(context);

	thread thrContext = thread([&]() {context.run(); });

	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

	asio::ip::tcp::socket socket(context);

	socket.connect(endpoint, ec);

	if (!ec) {
		cout << "[INFO] Connection successful!" << endl;
	}

	else {
		cout << "[INFO] Failed to connect to address:\n" << ec.message() << endl;
	}

	if (socket.is_open()) {

		GrabSomeData(socket); //so run func which outputs data as soon as parts of it are received
		//This func only runs socket is open aka a connection exists between client and server
		/*Call GrabSomeData in the beginning because we want it to be ON as soon as program starts so we can start outputting
		any received data from server*/

		string sRequest = //String which acts as client request to server
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec); //most data is sent to server
		//After writing data to server, we the client expect some response but can take time
		//so rest of code must execute after a delay to ensure that bytes have actually returned so it can be processed:

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);
		/*Instead of this we can use method in asio where we don't execute code below until we've read and confirmed
		there's incoming data*/

		context.stop();
		if (thrContext.joinable()) thrContext.join();

	}

		//No need for any of this as we're outputting received data from server asynchronously, so no need for regular read_some
		/*socket.wait(socket.wait_read);

		//Receiving data from server

		size_t bytes = socket.available();
		cout << "Bytes available: " << bytes << endl;

		if (bytes > 0) {

			vector<char> vBuffer(bytes);
			socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

			for (auto c: vBuffer) {
				cout << c;
			}

		}

	}*/

	return 0;

}