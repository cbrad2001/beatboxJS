// Code for driving client-based interactions on the index.html webpage.
console.log("Hi from beatbox_ui.js!")

// this is the client-side version of socket.io
const socket = io.connect()

$(document).ready(() => {
    $('#volumeid').focus();
    $('#status').text('Connecting...')

    socket.on('noConnect', () => {
        // Display message if there's no connection to Beatbox C app
        $('#status').text('No reply from Beatbox!')
    })

    socket.on('ack', () => {
        // Display message for successful ping
        $('#status').text('Beatbox is running!')
    })

    socket.on('uptime', (msg) => {
        $('#status').text('Beatbox has been running for ' + msg)
    })

    // Ping the JS server, which will then ping the C app
    socket.emit('uptime')
})

$('#modeNone').click(() => {
    var message = {
        mode : 0
    }
    socket.emit('mode', message)
})

$('#modeRock1').click(() => {
    var message = {
        mode : 1
    }
    socket.emit('mode', message)
})

$('#modeRock2').click(() => {
    var message = {
        mode : 2
    }
    socket.emit('mode', message)
})

$('#stopProgram').click(() => {
    socket.emit('stop', 'stop')
    $('#status').text('Shutting down beatbox...')
})

$('#volumeSet').click(() => {
    let volume = Number($('#volumeid').val())
    var message = {
        value: volume
    }
    socket.emit('volume', message)
})

$('#volumeUp').click(() => {
    let volume = Number($('#volumeid').val())
    if (volume < 100) {
        $('#volumeid').val(volume + 1)
    }
})

$('#volumeDown').click(() => {
    let volume = Number($('#volumeid').val())
    if (volume > 0) {
        $('#volumeid').val(volume - 1)
    }
})
