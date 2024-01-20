#pragma once
#include "net_common.h" //so we call the header file with all the headers we just created

//This is header file to just describe the message object which we can reuse in the main code file when creating a message object instance

using namespace std;

namespace net {

	template <typename T>
	struct message_header {
		T id{};
		uint32_t size = 0; //Size is meant to be size (in bytes) of body message, so initialize it to 0 when first creating object
	};

	template <typename T>
	struct message {
		message_header<T> header{};
		vector<uint8_t> body;

		size_t size() const //this is a const func as the logic in the func doesn't alter any variable values
		{
			return sizeof(message_header<T>) + body.size();
		}

		friend ostream& operator <<(ostream& os, const message<T>& msg) {
			os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size; //overrides cout<< command and gives output from
			//friend, and outputs message ID and message size 
			return os;
		}

		//Overriding << to contain message and data arguments 
		template<typename DataType>
		friend message<T>& operator << (message<T>& msg, const DataType& data) { //msg is the object, data is the extra content we want to import into the message

			/*Ensuring input is static (so memory alloc to static variable is done only once in program and throughout entirety
			of program, so when variable is called again, no new memory alloc, and current value assigned to the variable
			is carried over and reassigned*/
			static_assert(is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

			//Determine current size of body message
			size_t i = msg.body.size();

			//Resize body vector so there's space for additional bytes from the custom message
			msg.body.resize(msg.body.size() + sizeof(DataType));

			//Once extra memory is alloc then we can copy the extra content (data) into the msg
			memcpy(msg.body.data() + i, &data, sizeof(DataType));

			//Update size variable in msg header and assign it to the new message size
			msg.header.size = msg.size(); /*message object uses custom type message_header so we can access variables in
			struct of message_header eventhough object is instance of message*/

			//Return new message
			return msg;

		}

		//Overriding >> operator
		template <typename DataType>
		friend message<T>& operator >> (message<T>& msg, DataType& data) {
			/*mainly same as overriding << operator but we're removing stuff from the stream of data we receive so
			memory alloc should go down and body message should resize down/smaller*/
			static_assert(is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

			//We're removing data from end of vector
			size_t i = msg.body.size() - sizeof(DataType);

			memcpy(&data, msg.body.data() + i, sizeof(DataType));

			msg.body.resize(i);

			msg.header.size = msg.size();

			return msg;

		}



	};
}
}