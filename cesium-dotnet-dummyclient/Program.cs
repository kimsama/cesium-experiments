using SocketIOClient;
using System;
using System.Threading.Tasks;

class Program
{
  static async Task Main(string[] args)
  {
    var client = new SocketIOClient.SocketIO("http://localhost:4000");
    await client.ConnectAsync();

    Console.WriteLine("Please enter the GPS location in the format 'latitude, longitude'. For example: 37.80195697601734, -122.41674617751681");
    Console.WriteLine("To exit the program, type 'exit'.");

    while (true)
    {
      var input = Console.ReadLine(); // Receive user input

      if (string.Equals(input, "exit", StringComparison.OrdinalIgnoreCase))
      {
        Console.WriteLine("Exiting the program.");
        break; // Exit if 'exit' is entered
      }

      var parts = input.Split(','); // Split input by comma
      if (parts.Length != 2)
      {
        Console.WriteLine("Incorrect format. Please try again.");
        continue; // Request new input if the format is incorrect
      }

      if (double.TryParse(parts[0].Trim(), out double latitude) &&
          double.TryParse(parts[1].Trim(), out double longitude))
      {
        // Send GPS location data to the server
        await client.EmitAsync("gpsPosition", new { latitude, longitude });
        Console.WriteLine($"Location sent: Latitude {latitude}, Longitude {longitude}");
      }
      else
      {
        Console.WriteLine("The number format is incorrect. Please try again.");
      }
    }

    await client.DisconnectAsync();
  }
}
