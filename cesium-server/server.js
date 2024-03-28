const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

// Serve static files from the public directory
app.use(express.static(path.join(__dirname, 'public')));

io.on('connection', (socket) => {
  console.log('New client connected');

  socket.on('gpsPosition', (data) => {
    console.log('Received GPS position: ', data);
    io.emit('gpsPosition', data); // Emitting to all clients including Cesium
  });

  socket.on('disconnect', () => {
    console.log('Client disconnected');
  });
});


// Add a route that redirects to the index.html file
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

const port = process.env.PORT || 4000;
server.listen(port, () => console.log(`Listening on port ${port}`));
