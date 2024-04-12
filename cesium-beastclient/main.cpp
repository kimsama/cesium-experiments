#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = net::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

int main(int argc, char** argv) 
{
  try 
  {
    std::string_view host = argv[1];
    std::string_view port = argv[2];

    // Check command line arguments.
    if (argc != 3) 
    {
      host = "localhost";
      port = "4000";
    }

    // The io_context is required for all I/O
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver{ ioc };
    websocket::stream<tcp::socket> ws{ ioc };

    // Look up the domain name
    auto const results = resolver.resolve(host, port);

    // Make the connection on the IP address we get from a lookup
    auto ep = net::connect(ws.next_layer(), results);

    // Set a decorator to change the User-Agent of the handshake
    //ws.set_option(websocket::stream_base::decorator(
    //  [](websocket::request_type& req) {
    //    req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-cpp");
    //  }));

    // Perform the websocket handshake
    ws.handshake(host, "/");

    // Send a message with custom event type
    std::string message = R"({"type": "gpsPosition", "data": {"latitude": 37.7749, "longitude": -122.4194}})";
    ws.write(net::buffer(message));

    // This buffer will hold the incoming message
    beast::flat_buffer buffer;

    // Read a message into our buffer
    ws.read(buffer);
    std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;

    // Close the WebSocket connection
    ws.close(websocket::close_code::normal);
  }
  catch (std::exception const& e) 
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
