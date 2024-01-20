#include "net_message.h"

namespace net
{
    template<typename T> //Use templated classes as msg type was enum so custom types can be used by the classes
    class serverSide
    {
    public:
        //Constructor to take in port number
        serverSide(uint16_t inputPort)
            : m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
        {

        }
        //Destructor to end server:
        virtual ~server_interface()
        {
            Stop();
        }

        //Bool function to start the server:
        bool Start()
        {
            try
            {
                //Add asio context so that when server is looking for client it doesn't immediately end when nothing is found
                //so prime asio connection with a fake context so that when actual client comes it can still be active:
                waitforClient();
            }
            catch (exception& e)
            {
               //Exception is detected:
                cerr << "[SERVER] Exception: " << e.what() << "\n";
                return false;
            }

            std::cout << "[SERVER] Started!\n";
            return true;
        }

    //End server
        void Stop()
        {
         // Request the context to close
            m_asioContext.stop();

        //Write message to console stating that server has ended:
            std::cout << "[SERVER] Stopped!\n";
        }

        void loadClient()
        {
    //Wait for socket to be created between client and server
        (error_code ec, asio::ip::tcp::socket socket)
            {
            // Triggered by incoming connection request
            //The connection requesst is sent by client when running the trading bot
            if (!ec)
            {
                //Write message to console stating that server has connected to client:
                cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

                // Create a new connection to handle this client 
                shared_ptr<connection<T>> newconn = 
                    make_shared<connection<T>>(connection<T>::owner::server, 
                        m_asioContext, std::move(socket), m_qMessagesIn);
            }
            else
            {
                //If user was not joinable tell console that there was error:
                std::cout << "[SERVER] New Connection Error: " << ec.message() << "\n";
            }

            //after connection with a client has been established, re-run function below so it primes asio using fake context
            //for the next connection:
            waitforClient();
        });
        }

        // Send a message to a specific client
        void MessageClient(list<connection<T>> clients, const message<T>& msg)
        {
            //Remove most current client:
            client = clients.front();

            //Check whether client exists and is connected
            if (client && client->IsConnected())
            {
                client->Send(msg);
            }
        }
        
        // Send message to all clients
        void MessageAllClients(const message<T>& msg, list<connection<T>> clients)
        {
            bool bInvalidClientExists = false;

            // Iterate through all clients in container
            for (auto& client : clients)
            {
                //Repeat same logic as above function to check whether clients are present:
                if (client && client->IsConnected())
                {
                        client->Send(msg);
                }
            }

        // Force server to respond to incoming messages
        void Update(size_t nMaxMessages = -1, bool bWait = false)
        {
            //Finish code so that output messages from multiple clients are sent to one joint queue. 
        }
        }
}
