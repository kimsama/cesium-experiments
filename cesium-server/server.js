const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// asynchroneous message processing one at a time
const messageQueue = [];
let isProcessing = false;

app.use(express.static(path.join(__dirname, 'public')));

wss.on('connection', (ws) => {
  console.log('New client connected');

  ws.on('message', (data) => {
    messageQueue.push(data);
    processMessages(ws);
  });

  ws.on('close', () => {
    console.log('Client disconnected');
  });
});

async function processMessages(ws) {
  if (isProcessing) return;
  isProcessing = true;

  while (messageQueue.length > 0) {
    const data = messageQueue.shift();
    try {
      const message = JSON.parse(data);
      await handleMessage(ws, message);
    } catch (error) {
      console.error('Error handling message: ', error);
      ws.send(JSON.stringify({ type: 'error', data: 'Invalid JSON' }));
    }
  }

  isProcessing = false;
}

async function handleMessage(ws, message) {
  console.log(`Received ${message.type}: `, message.data);
  const response = { type: 'ack', data: `${message.type} received` };
  ws.send(JSON.stringify(response));

  if (message.type === 'gpsPosition') {
    await broadcastMessage(ws, message);
  }
}

async function broadcastMessage(ws, message) {
  const broadcastPromises = [];
  wss.clients.forEach(client => {
    if (client !== ws && client.readyState === WebSocket.OPEN) {
      broadcastPromises.push(new Promise((resolve, reject) => {
        client.send(JSON.stringify({ type: 'gpsPosition', data: message.data }), (error) => {
          if (error) reject(error);
          else resolve();
        });
      }));
    }
  });

  try {
    await Promise.all(broadcastPromises);
    console.log('Broadcast completed');
  } catch (error) {
    console.error('Error broadcasting message: ', error);
  }
}

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

const port = process.env.PORT || 4000;
server.listen(port, () => console.log(`Listening on port ${port}`));
