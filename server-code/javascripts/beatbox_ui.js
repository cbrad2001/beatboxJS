// Code for driving client-based interactions on the index.html webpage.
console.log("Hi from beatbox_ui.js!")

// this is the client-side version of socket.io
const socket = io.connect()

$(document).ready(() => {
    $('#volumeid').focus();
    $('#status').text('Connecting...')

    socket.on('noConnect', () => {
        // Display message if there's no connection to Beatbox C app
        noConnectDisplay()
    })

    socket.on('ack', () => {
        // Display message for successful ping
    })

    // Ping the JS server, which will then ping the C app
    socket.emit('ping')
})

function noConnectDisplay() {
    $('#status').text('No reply from Beatbox!')
    // $('#status-box').css('background-color', 'red')
}

function ackDisplay() {
    $('#status').text('Beatbox is running!')
}
