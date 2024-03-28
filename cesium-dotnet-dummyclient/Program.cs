using System;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

class Program
{
  static async Task Main(string[] args)
  {
    using var ws = new ClientWebSocket();
    await ws.ConnectAsync(new Uri("ws://localhost:4000/socket.io/?EIO=3&transport=websocket"), CancellationToken.None);

    Console.WriteLine("Connected to the server");

    double lat = 37.79968433190998;
    double lon = -122.42585293207298;

    for (int i = 0; i < 6; i++)
    {
      var latitude = lat + i * 0.1; // Example latitude
      var longitude = lon + i * 0.1; // Example longitude
      var gpsData = Encoding.UTF8.GetBytes($"42[\"gpsPosition\",{{\"latitude\":{latitude},\"longitude\":{longitude}}}]");

      await ws.SendAsync(new ArraySegment<byte>(gpsData), WebSocketMessageType.Text, true, CancellationToken.None);

      Console.WriteLine($"Sent GPS position: {latitude}, {longitude}");

      await Task.Delay(10000); // Wait for 10 seconds
    }

    await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, CancellationToken.None);
    Console.WriteLine("Disconnected from the server");
  }
}
