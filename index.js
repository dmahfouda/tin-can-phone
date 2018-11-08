var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
});

io.on('connect_error', function(connect_error){
  console.log('a user tried to connect');
  console.log(connect_error)
});

const ping = () => {
	console.log('led_on_before')
	io.send('led_on')
	console.log('led_on_after')
}

setInterval(ping, 5000)

http.listen(3000, function(){
  console.log('listening on *:3000');
});