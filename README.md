# cesium-experiments

A simple cesium server/client demo, for clients with different languages  than server to understand cesium with minimum effort for dummies. 
Not complicated but might be helpful to understand how to draw a vehicle'  path on a map with Cesium in real-time.  

- Client sends a GPS location in the format 'latitude, longitude' and send it to the Cesium server.
- Cesium server draws a poly line with the given path.
- Use socket.io for the connection.


### cesium-server
nodejs cesium server and client. It opens ort `4000` and listens.

### cesium-dotnet-dummyclient
.NET console client

Dependencies:
Use Nuget package manager to install socket.io

### cesium-cpp-dummyclient
Cpp console client.

Dependencies:
Use `vcpkg` to install socket.io cpp client. 
```
>vcpkg install socket-io-client
```

### Run

Specify cesium access tocken. 

Run cesium server
```
> node server.js
```

Now, you should see cesium renders a map.
Then, run .NET or cpp cesium client. 
Type any GPS coordinate. 
You can see each red line is drawn connected to the last GPS position. 

