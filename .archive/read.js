const mongoose = require('mongoose')
const fs = require('fs');
const wav = require('wav');
const Duplex = require('stream').Duplex;

mongoose.connect(process.env.MONGODB_URI || 'mongodb://localhost:27017/cans')

let db = mongoose.connection

db.on('error', console.error.bind(console, 'connection error:'))

let canSchema = new mongoose.Schema({
  canName: String,
  messages: Array
})

let Can = mongoose.model('Can', canSchema)

db.once('open', function(callback) {
  console.log('database connected');
  mongoose.connection.db.listCollections().toArray((err, names) => {
    console.log(`collection names: ${names}`); // [{ name: 'dbname.myCollection' }]
  })
  let name = "::ffff:192.168.1.234";
  Can.find({ canName: name }, (err, cans) => {
    if(err) {
      console.log(error);
    } else if(cans.length == 0) {
      console.log(`no cans found with name: ${name}`);
    } else {
      let result = "";
      let can = cans[0];
      for(let i = 0; i < can.messages.length; i++) {
        result += can.messages[i].sample;
      }
      let buffer = new Buffer(result, 'base64');
      let rstream = new Duplex();
      rstream.push(buffer);
      rstream.push(null);
      let transcoder = wav.Writer({ channels: 1, bitDepth:8, sampleRate: 7000});
      let wstream = fs.createWriteStream('blah.wav');
      rstream.pipe(transcoder).pipe(wstream);
    }
  })
})
