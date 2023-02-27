// Code for communicating with the Beatbox C program through UDP datagrams
// Note that this is different than the browser based JS that runs the UI of the web page.

// this is the server-side version of socket.io
var socketio = require('socket.io')
var io 

var dgram = require('dgram')
var udpSocket = dgram.createSocket('udp4')
var timeListenErrorTimer

const DEST_PORT = 12345

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

    // message means trigger on any datagram received, where msg is the text content
    udpSocket.on('message', (msg) => {
        // console.log('NodeJS UDP received: ' + msg)
        var splitMsg = msg.toString().split("|")

        if (splitMsg[0] == 'uptime') {
            clearTimeout(timeListenErrorTimer)

            var totalSeconds = splitMsg[1]

            var toSend = {
                reply: totalSeconds
            }

            io.sockets.emit('uptime', toSend)

            timeListenErrorTimer = setTimeout(() => {
                // console.log('Beatbox timed out! No response from it.')
                io.sockets.emit("noConnect")
            }, 2000)
        }
        else if (splitMsg[0] == 'mode') {
            // var status = splitMsg[1]
            var mode = splitMsg[2]
            var reply = splitMsg[3]

            var toSend = {
                mode: mode,
                reply: reply
            }
            // console.log('Sending websocket packet for mode!');
            io.sockets.emit('mode', toSend)
        }
        else if (splitMsg[0] == 'volume') {
            var status = splitMsg[1]
            if (status === 't')
            {
                var volume = Number(splitMsg[2])
                var reply = splitMsg[3]

                var toSend = {
                    status: status,
                    vol: volume,
                    reply: reply
                }
                // console.log('Sending websocket packet for volume!');
                io.sockets.emit('volume', toSend)
            }
            else if (status === 'f')
            {
                var reply = splitMsg[2]
                var toSend = {
                    status: status,
                    reply: reply
                }
                // console.log('Sending websocket packet for volume!');
                io.sockets.emit('volume', toSend)
            }
            else {
                console.log("Something unexpected happened with the status where Beatbox sent: " + status);
            }
        }
        else if (splitMsg[0] == "tempo") {
            var status = splitMsg[1]
            if (status === 't')
            {
                var tempo = Number(splitMsg[2])
                var reply = splitMsg[3]

                var toSend = {
                    status: status,
                    tempo: tempo,
                    reply: reply
                }
                // console.log('Sending websocket packet for tempo!');
                io.sockets.emit('tempo', toSend)
            }
            else if (status === 'f')
            {
                var reply = splitMsg[2]
                var toSend = {
                    status: status,
                    reply: reply
                }
                // console.log('Sending websocket packet for tempo!');
                io.sockets.emit('tempo', toSend)
            }
            else {
                console.log("Something unexpected happened with the status where Beatbox sent: " + status);
            }
        }
        else if (splitMsg[0] == "sound") {
            var reply = splitMsg[1]
            var toSend = {
                reply: reply
            }
            // console.log('Sending websocket packet for sound!');
            io.sockets.emit('sound', toSend)
        }
        else if (splitMsg[0] == "stop") {
            var reply = splitMsg[1]
            var toSend = {
                reply: reply
            }
            // console.log('Sending websocket packet for stop!');
            io.sockets.emit('stop', toSend)
        }
        else if (splitMsg[0] == "status") {
            // console.log('DEBUG: status sent is ' + splitMsg)
            var toSend = {
                mode: splitMsg[1],
                vol: splitMsg[2],
                bpm: splitMsg[3]
            }
            // console.log('Sending websocket packet for status!');
            io.sockets.emit('status', toSend)
        }
        else if (splitMsg[0] == "unknown") {
            var reply = splitMsg[1]
            var toSend = {
                reply: reply
            }
            // console.log('Sending websocket packet for unknown!');
            io.sockets.emit('unknown', toSend)
        }
        else {
            console.log("DEBUG: Beatbox sent something unexpected: " + msg.toString())
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
    socket.on('init', () => {
        console.log('Client connected!')
        timeListenErrorTimer = setTimeout(() => {
            // console.log('Sent a noConnect!')
            socket.emit('noConnect')
        }, 2000)
        socket.emit('jsAck')

        var toSend = 'status'
        udpSocket.send(toSend, DEST_PORT, 'localhost')
    })

    socket.on('mode', (data) => {
        var value = data.mode
        var toSend = 'mode ' + value
        // console.log('DEBUG: sending UDP packet to change mode to ' + value)
        udpSocket.send(toSend, DEST_PORT, 'localhost')
    })

    socket.on('stop', (data) => {
        udpSocket.send('stop', DEST_PORT, 'localhost')
    })

    socket.on('volume', (data) => {
        var value = data.volume
        var toSend = 'volume ' + value
        udpSocket.send(toSend, DEST_PORT, 'localhost')
    })

    socket.on('sound', (data) => {
        var value = data.sound
        var toSend = 'sound ' + value
        udpSocket.send(toSend, DEST_PORT, 'localhost')
    })

    socket.on('tempo', (data) => {
        var value = data.bpm
        var toSend = 'tempo ' + value
        udpSocket.send(toSend, DEST_PORT, 'localhost')
    })

    socket.on('status', (data) => {
        udpSocket.send('status', DEST_PORT, 'localhost')
    })
}