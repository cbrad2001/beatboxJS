// Code for communicating with the Beatbox C program through UDP datagrams
// Note that this is different than the browser based JS that runs the UI of the web page.

// this is the server-side version of socket.io
var socketio = require('socket.io')
var io 

var dgram = require('dgram')
var udpSocket = dgram.createSocket('udp4')
var errorTimer

// Export a listener function for server.js to use
exports.listen = (server, udpPort) => {
    io = socketio(server)
    udpSocket.bind(udpPort)

    // Removed in later version of Socket.io
    // io.set('log level 1')

    // The lambda here is an event driven function that triggers on a websocket packet receive
    io.on('connection', (socket) => {
        handleSocketCommand(socket)
    })

    udpSocket.on('message', (msg) => {
        clearTimeout(errorTimer)
        console.log('NodeJS UDP received: ' + msg)
        // io.sockets.emit('ack')

        if (msg.toString().substring(0, 6) == 'uptime') {
            let time = msg.toString().substring(6)
            io.sockets.emit('uptime', time)
        }
    })

    udpSocket.on('listening', () => {
        console.log('Listening to UDP on port ' + udpPort)
    })

    udpSocket.on('error', (err) => {
        console.log('UDP error: ' + err)
    })
}

// Function for handling commands from the client-side js
function handleSocketCommand(socket) {
    socket.on('uptime', () => {
        errorTimer = setTimeout(() => {
            socket.emit("noConnect")
            console.log('No reply from Beatbox!')
        }, 5000)
        // Ping the C program here
        udpSocket.send('uptime', 12345, '127.0.0.1')
    })

    socket.on('mode', (data) => {
        errorTimer = setTimeout(() => {
            socket.emit("noConnect")
            console.log('No reply from Beatbox!')
        }, 5000)
        var value = data.mode
        var toSend = 'mode ' + value
        udpSocket.send(toSend, 12345, '127.0.0.1')
    })

    socket.on('stop', (data) => {
        udpSocket.send('stop', 12345, '127.0.0.1')
    })
}