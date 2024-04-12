#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class websocket_client 
{
public:
  websocket_client(net::io_context& ioc)
    : resolver_(net::make_strand(ioc)), ws_(net::make_strand(ioc)) {}

  void connect(const std::string& host, const std::string& port) 
  {
    resolver_.async_resolve(host, port,
      beast::bind_front_handler(&websocket_client::on_resolve, this));
  }

private:
  tcp::resolver resolver_;
  websocket::stream<beast::tcp_stream> ws_;
  beast::flat_buffer buffer_;  // Buffer for incoming messages

  void on_resolve(beast::error_code ec, tcp::resolver::results_type results) 
  {
    if (ec) return fail(ec, "resolve");

    ws_.next_layer().async_connect(*results.begin(),
      beast::bind_front_handler(&websocket_client::on_connect, this));
  }

  void on_connect(beast::error_code ec) 
  {
    if (ec) return fail(ec, "connect");

    ws_.async_handshake("localhost", "/",
      beast::bind_front_handler(&websocket_client::on_handshake, this));
  }

  void on_handshake(beast::error_code ec) 
  {
    if (ec) return fail(ec, "handshake");

    // Send a message
    ws_.async_write(net::buffer(std::string(R"({"type": "gpsPosition", "data": {"latitude": 37.7749, "longitude": -122.4194}})")),
      beast::bind_front_handler(&websocket_client::on_write, this));
  }

  void on_write(beast::error_code ec, std::size_t bytes_transferred) 
  {
    if (ec) return fail(ec, "write");

    ws_.async_read(buffer_,
      beast::bind_front_handler(&websocket_client::on_read, this));
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) 
  {
    if (ec) return fail(ec, "read");

    std::cout << "Received: " << beast::make_printable(buffer_.data()) << std::endl;
    buffer_.consume(buffer_.size()); // Clear the buffer

    // Continue reading messages here, or close handshake
    //ws_.async_close(websocket::close_code::normal,
    //  beast::bind_front_handler(&websocket_client::on_close, this));
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

  // Instead of running io_context::run() in your game loop, 
  // you can use io_context::poll() or io_context::poll_one().
  // These methods execute handlers that are ready to run, 
  // without blocking to wait for new events.
  // This allows your game loop to remain responsive and continue processing other tasks.
  while (true) 
  {
    // get stdin input
    std::string input;
    std::getline(std::cin, input);
    // exit the game loop if the user types "exit"
    if (input == "exit")
      break;

    // any game logic here
    //...

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

  while (true)
  {
    // get stdin input
    std::string input;
    std::getline(std::cin, input);
    // exit the game loop if the user types "exit"
    if (input == "exit") 
      break;
     
    // any game logic here
  }

  // Ensure to stop the io_context and join the thread on game exit
  ioc.stop();
  networkThread.join();

  return 0;
}
#endif
