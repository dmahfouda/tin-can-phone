const fs = require("fs");
const Lame = require("node-lame").Lame;

const encoder = new Lame({
    output: "./blah.mp3",
    bitrate: 192
}).setFile("./blah.wav");

encoder
    .encode()
    .then(() => {
        // Encoding finished
    })
    .catch(error => {
        // Something went wrong
    });

// const lamejs = require("lamejs");
//
// fs.readFile("audio.wav", (err, data) => {
//     if (err) {
//         console.log(err);
//         process.exit();
//     }
//
//     let wav = lamejs.WavHeader.readHeader(new DataView(data.buffer));
//     console.log(wav)
//
//     let encoder = new lamejs.Mp3Encoder(wav.channels, wav.sampleRate, 128);
//     let samples = new Int16Array(data, wav.dataOffset, wav.dataLen / 2);
//
//     let buffer = new Int8Array();
//     let remaining = samples.length;
//     let maxSamples = 1152;
//     for (let i = 0; remaining >= maxSamples; i += maxSamples) {
//         let mono = samples.subarray(i, i + maxSamples);
//         let mp3buf = encoder.encodeBuffer(mono);
//         if (mp3buf.length > 0) {
//             let old = buffer;
//             buffer = new Int8Array(old.length + mp3buf.length);
//             buffer.set(old);
//             buffer.set(new Int8Array(mp3buf), old.length);
//         }
//         remaining -= maxSamples;
//     }
//
//     let d = encoder.flush();
//     if(d.length > 0) {
//         let old = buffer;
//         buffer = new Int8Array(old.length + d.length);
//         buffer.set(old);
//         buffer.set(new Int8Array(d), old.length);
//     }
//
//     console.log('done encoding, size=', buffer.length);
//
//     fs.writeFile("audio.mp3", buffer, (err) => {
//         if (err) {
//             console.log(err);
//             process.exit();
//         }
//     });
// });


/*
var fs = require('fs')
var lamejs = require('lamejs')

var channels = 1; //1 for mono or 2 for stereo
var sampleRate = 44100; //44.1khz (normal mp3 samplerate)
var kbps = 128; //encode 128kbps mp3
var mp3encoder = new lamejs.Mp3Encoder(channels, sampleRate, kbps);
var mp3Data = [];

var samples = new Int16Array(44100); //one second of silence (get your data from the source you have)
var sampleBlockSize = 1152; //can be anything but make it a multiple of 576 to make encoders life easier

var mp3Data = [];

for (var i = 0; i < samples.length; i += sampleBlockSize) {
  sampleChunk = samples.subarray(i, i + sampleBlockSize);
  var mp3buf = mp3encoder.encodeBuffer(sampleChunk);
  if (mp3buf.length > 0) {
    mp3Data.push(mp3buf);
  }
}

var mp3buf = mp3encoder.flush();   //finish writing mp3

if (mp3buf.length > 0) {
  mp3Data.push(new Int8Array(mp3buf));
}

var blob = new Blob(mp3Data, {type: 'audio/mp3'});
var url = window.URL.createObjectURL(blob);

console.log('MP3 URl: ', url);
*/
