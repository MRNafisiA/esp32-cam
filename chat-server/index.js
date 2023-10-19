const { log } = require("console");
const express = require("express");
const { param } = require("express/lib/request");
const http = require("http");
const WebSocket = require("ws");

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const checkToken = (token) => {
  return token == '1234';
};

const connections = [];

wss.on("connection", (ws, request) => {
  const url = new URL(request.url, `http://${request.headers.host}`);

  const query = url.searchParams;

  if (checkToken(query.get('token'))) {
    ws.send("You login successfully.");

    const id = generateUniqueID()

    ws.id = id;
    ws.clinetId = query.get('clientId')

    connections.push({
      client: query.get('clientId'),
      ws: ws,
      id: id,
      connectedAt: new Date(),
    });
  } else {
    ws.send('You have not permission.')
    ws.close();
    return;
  }

  ws.on("message", (data) => {
    const parseData = JSON.parse(data.toString());
    wss.clients.forEach((client) => {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(`Client ${ws.clinetId}: ${parseData.message}`);
      }
    });
  });
});

app.get("/:id", (req, res) => {
  res.sendFile(__dirname + `/client_${req.params.id}.html`);
});

server.listen(3000, () => {
  console.log("Server is running on http://localhost:3000");
});


function generateUniqueID() {
    const timestamp = new Date().getTime();
    const random = Math.floor(Math.random() * 1000);
  
    return `${timestamp}-${random}`;
  }