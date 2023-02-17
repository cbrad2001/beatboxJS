// Code for communicating with the Beatbox C program through UDP datagrams
// Note that this is different than the browser based JS that runs the UI of the web page.

// this is the server-side version of socket.io
var socketio = require('socket.io')
var io 

// Export a listener function for server.js to use
exports.listen = (server) => {
    io = socketio(server)

    // Removed in later version of Socket.io
    // io.set('log level 1')

    // The lambda here is an event driven function that triggers on a packet receive
    io.on('connection', (socket) => {
        handleCommand(socket)
    })
}

// Function for handling commands from the client-side js
function handleCommand(socket) {
    socket.on('ping', () => {
        var errorTimer = setTimeout(() => {
            socket.emit("noConnect")
            console.log('No reply from Beatbox!')
        }, 5000)
        // Ping the C program here
        console.log('Pinging beatbox...')

        // clearTimeout(errorTimer)
    })
}