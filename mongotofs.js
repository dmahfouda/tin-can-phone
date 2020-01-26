const mongoose = require('mongoose')
const fs = require('fs');
const wav = require('wav');
const Duplex = require('stream').Duplex;
var lamejs = require('lamejs')
const Lame = require("node-lame").Lame;

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

      //
      // read from db and write to file
      //
      // const encoder = new Lame({
      //     output: "./new_audio.mp3",
      //     raw: true,
      //     bitwidth: 8,
      //     mode: "m",
      //     sfreq: 8,
      //     bitrate: 40
      // }).setBuffer(buffer);
      //
      // encoder
      //     .encode()
      //     .then(() => {
      //         console.log("success")
      //     })
      //     .catch(error => {
      //         // Something went wrong
      //         console.log(error)
      //     });

      // read from db and write to buffer
      const encoder = new Lame({
        output: "buffer",
        raw: true,
        bitwidth: 8,
        mode: "m",
        sfreq: 8,
        bitrate: 40
      }).setBuffer(buffer);

      encoder.encode().then(() => {
        console.log("success");
        let result = encoder.getBuffer();
        fs.writeFileSync("result.mp3", result, (err) => {
            console.log(err);
        });
        process.exit();
      }).catch(error => {
        console.log(error)
        process.exit();
      });
    }
  })
})
