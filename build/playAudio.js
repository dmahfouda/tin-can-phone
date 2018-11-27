const socket = io();

socket.emit('register')
console.log('test')

var player = new PCMPlayer({
    encoding: '8bitInt',
    channels: 2,
    sampleRate: 8000,
    flushingTime: 2000
});

socket.on('audio', () => {
	console.log('getting audio')
	player.feed(aud)
})

// var socket = io();

// socket.emit('toBrowser')

// var player = new PCMPlayer({
//         encoding: '16bitInt',
//         channels: 2,
//         sampleRate: 8000,
//         flushingTime: 2000
// });

// socket.on('audio', (aud) => {
// 	player.feed(aud)
// })
   
// var ws = new WebSocket(socketURL);
//    ws.binaryType = 'arraybuffer';
//    ws.addEventListener('message',function(event) {
//         var data = new Uint8Array(event.data);
//         player.feed(data);
//    });