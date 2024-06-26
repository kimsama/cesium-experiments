#include <iostream>
#include <string>
#include <deque>
#include <mutex>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

/// <summary>
/// websocket_client class that sends GPS position data to the server
/// </summary>
class websocket_client 
{
public:
  websocket_client(net::io_context& ioc)
    : _resolver(net::make_strand(ioc)), 
      _ws(net::make_strand(ioc))
  {}

  void connect(const std::string& host, const std::string& port) 
  {
    _resolver.async_resolve(host, port,
      beast::bind_front_handler(&websocket_client::on_resolve, this));
  }

  void send(const std::string& message)
  {
    // The specified handler is scheduled for execution as thread-safe manner. 
    net::post(_ws.get_executor(),
      beast::bind_front_handler(&websocket_client::on_send, this, message));
  }

private:
  tcp::resolver _resolver;
  websocket::stream<beast::tcp_stream> _ws;
  std::deque<std::string> _message_queue;

  // Flag to track if a read/write operation is in progress
  bool _ready_to_read = false;
  bool _write_in_progress = false;

  // Buffer for incoming messages
  beast::flat_buffer buffer_;  

  void on_send(const std::string& message)
  {
    if (_write_in_progress)
    {
      _message_queue.push_back(message);
    }
    else
    {
      _write_in_progress = true;
      _ws.async_write(net::buffer(message),
        beast::bind_front_handler(&websocket_client::on_write, this));
    }
  }

  void on_resolve(beast::error_code ec, tcp::resolver::results_type results) 
  {
    if (ec) return fail(ec, "resolve");

    _ws.next_layer().async_connect(*results.begin(),
      beast::bind_front_handler(&websocket_client::on_connect, this));
  }

  void on_connect(beast::error_code ec) 
  {
    if (ec) return fail(ec, "connect");

    _ws.async_handshake("localhost", "/",
      beast::bind_front_handler(&websocket_client::on_handshake, this));
  }

  void on_handshake(beast::error_code ec) 
  {
    if (ec) return fail(ec, "handshake");

    // Send a message
    send(std::string(R"({"type": "handshake", "data": "Hello"})"));

    // Initiate the first read operation
    _ready_to_read = true;
    _ws.async_read(buffer_,
      beast::bind_front_handler(&websocket_client::on_read, this));
  }

  void on_write(beast::error_code ec, std::size_t bytes_transferred)
  {
    if (ec) return fail(ec, "write");

    _write_in_progress = false;

    if (!_message_queue.empty())
    {
      _write_in_progress = true;
      _ws.async_write(net::buffer(_message_queue.front()),
        beast::bind_front_handler(&websocket_client::on_write, this));
      _message_queue.pop_front();
    }
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) 
  {
    if (ec) return fail(ec, "read");

    // Mark the read operation as completed
    _ready_to_read = false; 

    std::cout << "Received: " << beast::make_printable(buffer_.data()) << std::endl;
    // Clear the buffer
    buffer_.consume(buffer_.size()); 

    // Initiate a new read operation if one is not already in progress
    if (!_ready_to_read)
    {
      _ready_to_read = true;
      _ws.async_read(buffer_,
        beast::bind_front_handler(&websocket_client::on_read, this));
    }
  }

  void on_close(beast::error_code ec) 
  {
    if (ec) return fail(ec, "close");

    std::cout << "Connection closed" << std::endl;
  }

  void fail(beast::error_code ec, char const* what) 
  {
    std::cerr << what << ": " << ec.message() << std::endl;
  }
};

//#define _MY_PLAIN_ 
//#define _MY_POLLING_ 
#define _MY_NETWORK_THREAD_

#ifdef _MY_PLAIN_
int main() 
{
  net::io_context ioc;
  websocket_client client(ioc);
  client.connect("localhost", "4000");

  ioc.run();
  return 0;
}
#elif defined(_MY_POLLING_)
int main()
{
  net::io_context ioc;
  websocket_client client(ioc);
  client.connect("localhost", "4000");

  // get stdin input
  std::string input;

  // Instead of running io_context::run() in your game loop, 
  // you can use io_context::poll() or io_context::poll_one().
  // These methods execute handlers that are ready to run, 
  // without blocking to wait for new events.
  // This allows your game loop to remain responsive and continue processing other tasks.
  while (true) 
  {
    std::getline(std::cin, input);

    // exit the game loop if the user types "exit"
    if (input == "exit")
    {
      std::cout << "Exiting the program." << std::endl;
      break;
    }

    // any game logic here
    //...
    size_t commaPos = input.find(',');
    if (commaPos == std::string::npos || commnaPos == 0 || commaPos = input.length() - 1)
    {
      std::cout << "Incorrect format. . Please enter latitude and longitude separated by a comma." << std::endl;
      continue;
    }

    std::string latStr = input.substr(0, commaPos);
    std::string lonStr = input.substr(commaPos + 1);

    try
    {
      // Using nlohmann::json to create the JSON object
      nlohmann::json j;
      j["type"] = "gpsPosition";
      j["data"] = {
          {"latitude", latStr},
          {"longitude", lonStr}
      };
      client.send(j.dump());
    }
    catch (std::invalid_argument const& ia)
    {
      std::cerr << "The number format is incorrect:" << ia.what() << std::endl;
    }

    // Run the io_context to process ready handlers
    ioc.poll();

    // About 60 FPS
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); 
  }
  return 0;
}
#elif defined(_MY_NETWORK_THREAD_)
int main()
{
  net::io_context ioc;
  websocket_client client(ioc);
  client.connect("localhost", "4000");

  std::thread networkThread([&ioc]() {
    ioc.run();
  });

  // get stdin input
  std::string input;

  while (true)
  {
    std::getline(std::cin, input);

    // exit the game loop if the user types "exit"
    if (input == "exit")
    {
      std::cout << "Exiting the program." << std::endl;
      break;
    }

    size_t commaPos = input.find(',');
    if (commaPos == std::string::npos || commaPos == 0 || commaPos == input.length() - 1)
    {
      std::cout << "Incorrect format. . Please enter latitude and longitude separated by a comma." << std::endl;
      continue;
    }

    std::string latStr = input.substr(0, commaPos);
    std::string lonStr = input.substr(commaPos + 1);

    try
    {
      // Using nlohmann::json to create the JSON object
      nlohmann::json j;
      j["type"] = "gpsPosition";
      j["data"] = {
          {"latitude", latStr},
          {"longitude", lonStr}
      };
      client.send(j.dump());
    }
    catch (std::invalid_argument const& ia)
    {
      std::cerr << "The number format is incorrect:" << ia.what() << std::endl;
    }
  }

  // Ensure to stop the io_context and join the thread on game exit
  ioc.stop();
  networkThread.join();

  return 0;
}
#endif
