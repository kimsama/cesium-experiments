# cesium-experiments

A simple cesium server/client demo, for clients with different languages  than server 
 
- Client send a GPS location in the format 'latitude, longitude' and send it to the Cesium server.
- Cesium server draws a poly line with the given path.
- Use socket.io for the connection.

### cesium-server
nodejs cesium server and client

### cesium-dotnet-dummyclient
.NET console client

Dependencies:
Use Nuget package manager to install socket.io

### cesium-cpp-dummyclient
cpp consol client

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

Run .NET or cpp cesium client. 



