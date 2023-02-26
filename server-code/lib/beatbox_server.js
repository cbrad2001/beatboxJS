// Code for communicating with the Beatbox C program through UDP datagrams
// Note that this is different than the browser based JS that runs the UI of the web page.

// this is the server-side version of socket.io
var socketio = require('socket.io')
var io 

var dgram = require('dgram')
var udpSocket = dgram.createSocket('udp4')
var timeListenErrorTimer

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
        // io.sockets.emit('ack')
        var splitMsg = msg.toString().split("|")

        if (splitMsg[0] == 'uptime') {
            clearTimeout(timeListenErrorTimer)

            var totalSeconds = splitMsg[1]

            var toSend = {
                reply: totalSeconds
            }

            io.sockets.emit('uptime', toSend)

            timeListenErrorTimer = setTimeout(() => {
                io.sockets.emit("noConnect")
            }, 1100)
        }
        else if (splitMsg[0] == 'mode') {
            // var status = splitMsg[1]
            var mode = splitMsg[2]
            var reply = splitMsg[3]

            var toSend = {
                mode: mode,
                reply: reply
            }
            io.sockets.emit('mode', toSend)
        }
        else if (splitMsg[0] == 'volume') {
            var status = splitMsg[0]
            if (status === 't')
            {
                var volume = Number(splitMsg[2])
                var reply = splitMsg[3]

                var toSend = {
                    volume: volume,
                    reply: reply
                }
                io.sockets.emit('volume', toSend)
            }
            else if (status === 'f')
            {
                var reply = splitMsg[2]
                var toSend = {
                    reply: reply
                }
                io.sockets.emit('volume', toSend)
            }
            else {
                console.log("Something unexpected happened with the status where Beatbox sent: " + status);
            }
        }
        else if (splitMsg[0] == "tempo") {
            var status = splitMsg[0]
            if (status === 't')
            {
                var tempo = Number(splitMsg[2])
                var reply = splitMsg[3]

                var toSend = {
                    tempo: tempo,
                    reply: reply
                }
                io.sockets.emit('tempo', toSend)
            }
            else if (status === 'f')
            {
                var reply = splitMsg[2]
                var toSend = {
                    reply: reply
                }
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
            io.sockets.emit('sound', toSend)
        }
        else if (splitMsg[0] == "stop") {
            var reply = splitMsg[1]
            var toSend = {
                reply: reply
            }
            io.sockets.emit('stop', toSend)
        }
        else if (splitMsg[0] == "unknown") {
            var reply = splitMsg[1]
            var toSend = {
                reply: reply
            }
            io.sockets.emit('unknown', toSend)
        }
        else {
            console.log("Beatbox sent something unexpected: " + msg.toString())
        }
    })

    udpSocket.on('listening', () => {
        console.log('Listening to UDP on port ' + udpPort)
    })

    udpSocket.on('error', (err) => {
        console.log('UDP error: ' + err)
    })

    // After setting up listeners, set timeout for when Beatbox doesn't send a message with uptime info.
    timeListenErrorTimer = setTimeout(() => {
        socket.emit("noConnect")
    }, 1100)
}

// Function for handling commands from the client-side js
function handleSocketCommand(socket) {
    socket.on('init', () => {
        socket.emit('jsAck')
    })

    socket.on('mode', (data) => {
        var value = data.mode
        var toSend = 'mode ' + value
        udpSocket.send(toSend, 12345, '127.0.0.1')
    })

    socket.on('stop', (data) => {
        udpSocket.send('stop', 12345, '127.0.0.1')
    })

    socket.on('volume', (data) => {
        var value = data.volume
        var toSend = 'volume ' + value
        udpSocket.send(toSend, 12345, '127.0.0.1')
    })
}