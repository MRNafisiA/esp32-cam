const http = require('http');
const bodyParser = require('body-parser');
const express = require('express');
const WebSocket = require('ws');

const app = express();
const port = 9000;
const server = http.createServer(app);
app.use(bodyParser.raw({type: 'image/jpeg', limit: '10mb'}));
const wss = new WebSocket.Server({server});

let lastFrame;

app.post('/receive-image', (req, res) => {
    const imageData = req.body;

    if (!imageData) {
        console.log(typeof imageData);
        res.status(400).json({error: 'Invalid image data'});
        return;
    } else {
        lastFrame = imageData;
    }

    console.log(typeof imageData);

    res.status(200).json({message: 'Frame received and saved'});
});

wss.on('connection', (ws) => {
    const helloInterval = setInterval(() => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(lastFrame);
        }
    }, 10);

    ws.on('close', () => {
        clearInterval(helloInterval);
    });
});


// Start the server
server.listen(port, () => {
    console.log(`Server listening on port ${port}`);
});
