var express = require('express')
var app = express()
var http = require('http').Server(app)
var io = require('socket.io')(http)

app.use(express.static('build'))

const mongoose = require('mongoose')
mongoose.connect(process.env.MONGODB_URI || 'mongodb://localhost:27017/cans')

let db = mongoose.connection
db.on('error', console.error.bind(console, 'connection error:'))
db.once('open', function(callback) {
  console.log('database connected')
  mongoose.connection.db.listCollections().toArray((err, names) => {
    console.log(`collection names: ${names}`); // [{ name: 'dbname.myCollection' }]
  })
})

let canSchema = new mongoose.Schema({
  canName: String,
  messages: Array
})

let pairs = {
  '::ffff:10.0.20.221': '::ffff:10.0.19.155',
  '::ffff:10.0.19.155': '::ffff:10.0.20.221'
}

// let canPairSchema = new mongoose.Schema({
//   can1: String,
//   can2: String
// }, {collection :'canPairs'})

let Can = mongoose.model('Can', canSchema)
// let CanPairs = mongoose.model('canPair', canPairSchema, 'canPairs')

// CanPairs.find({}, (err, data) => {
//   if (!err) {
//     console.log(`data: ${data.json}`)
//   }
// })

var client

io.on('connection', function(socket){
  console.log('a user connected');
  let address = socket.handshake.address
  console.log(`new ip address: ${address}`)
  Can.find( {canName:address}, (err, cans) => {
    console.log(`cans1: ${cans}`)
    if (err) { 
      return console.error(err)
    } else {
        if (cans.length == 0) {
          console.log('no can in database')
          console.log(cans)
          let can = new Can({canName:address})
          can.save( err => {
              if (err) {
                console.log(err)
              } else {
                console.log('we think we saved it')
                socket.emit(can.messages.length)
              }
            })
        } else {
          socket.emit('messagelength', cans[0].messages.length)
          console.log('this is where we will send back can state from server')
          console.log(`cans: ${cans}`)
        }
    }
  })

  socket.on('messageType', (msg) => { 
    var data = new Uint8Array(msg.sample);
    if (client) {
      client.emit('audio', data)
    }
    // player.feed(data);
  })

  socket.on('incomingMessage', (msg) => {
    console.log(`msg: ${msg}`)
    Can.update({canName:pairs[address]}, {messages:[msg]}, (err, raw) => {
      // console.log(`cans2: ${cans}`)
      if (!err) {
        // cans.messages.push(msg.message)
        // cans.save(err => {
        //   if (!err) {
        //     console.log('we think we saved a message')
        //   }
        // })
        console.log(raw)
      } 
      // else {
        // console.log('no find error') 
      // } 
    }) 
  })
  
  // socket.send('hello')
	socket.on('disconnect', function(){
		console.log('disconnected: ' + socket.id);
	})

  socket.on('chat message', function(msg){
    // console.log('message: ' + msg);
    io.send(msg)
  })

  socket.on('register', () => {
    console.log('registered');
    client = socket.conn
    // socket.emit('ack')
  })
  
  //ping();
})



// io.on('connect_error', function(connect_error){
//   console.log('a user tried to connect');
//   console.log(connect_error)
// });

const ping = () => {
	console.log('led_on_before')
	io.send('led_on')
	console.log('led_on_after')
}

// setInterval(ping, 5000)

http.listen(3000, function(){
  console.log('listening on *:3000');
});