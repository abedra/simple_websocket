const ws = require('ws');

const wss = new ws.WebSocketServer({ port: 8080 });
let count = 0;

wss.on('connection', function connection(ws) {
  setInterval(() => {
    ws.send(count)
    count += 1
  }, 1000);
});
