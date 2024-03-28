#include <sio_client.h>
#include <iostream>
#include <functional>
#include <string>

/// <summary>
/// input a GPS location in the format 'latitude, longitude' and send it to the server.
/// </summary>
int main(int argc, char* argv[]) 
{
  sio::client h;
  h.connect("http://localhost:4000");

  std::cout << "Please enter the GPS location in the format 'latitude, longitude'. For example: 37.80195697601734, -122.41674617751681" << std::endl;
  std::cout << "To exit the program, type 'exit'." << std::endl;

  std::string input;
  while (true) 
  {
    std::getline(std::cin, input);

    if (input == "exit") 
    {
      std::cout << "Exiting the program." << std::endl;
      break;
    }

    size_t commaPos = input.find(',');
    if (commaPos == std::string::npos || commaPos == 0 || commaPos == input.length() - 1) 
    {
      std::cout << "Incorrect format. Please try again." << std::endl;
      continue;
    }

    std::string latStr = input.substr(0, commaPos);
    std::string lonStr = input.substr(commaPos + 1);

    try 
    {
      double latitude = std::stod(latStr);
      double longitude = std::stod(lonStr);

      // Create a message object to send
      sio::message::ptr msg = sio::object_message::create();
      msg->get_map()["latitude"] = sio::double_message::create(latitude);
      msg->get_map()["longitude"] = sio::double_message::create(longitude);

      h.socket()->emit("gpsPosition", msg);

      std::cout << "Location sent: Latitude " << latitude << ", Longitude " << longitude << std::endl;
    }
    catch (const std::invalid_argument& ia) {
      std::cout << "The number format is incorrect. Please try again." << std::endl;
    }
  }

  h.sync_close();
  h.clear_con_listeners();
}
