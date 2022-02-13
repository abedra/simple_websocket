const ws = require('ws');

const wss = new ws.WebSocketServer({ port: 8080 });

const start = ws => {
  console.log("Counting down")

  for (i = 0; i < 10; i++) {
    ws.send(i)
  }

  console.log("Countdown complete, closing connection")
  ws.close()
}

wss.on('connection', function connection(ws) {
  start(ws)
});
