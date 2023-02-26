// Code for driving client-based interactions on the index.html webpage.
console.log("Hi from beatbox_ui.js!")

// this is the client-side version of socket.io
const socket = io.connect()

var errorTimer

$(document).ready(() => {
    $('#volumeid').focus();
    $('#status').text('Connecting...')

    // Set up the listeners for message types
    socket.on('noConnect', () => {
        // Display message if there's no connection to Beatbox C app
        $('#error-header').text('Beatbox Error')
        $('#error-text').text('No response from Beatbox! Is it running?')
    })

    socket.on('jsAck', () => {
        // Display message for successful ping to JS server
        clearTimeout(errorTimer)
    })

    socket.on('uptime', (msg) => {
        let seconds = Number(msg)
        let minutes = Math.trunc(seconds / 60)
        seconds %= 60
        let hours = Math.trunc(minutes / 60)
        minutes %= 60

        $('#status').text('Beatbox has been running for ' + hours + ':' + minutes + ':' + seconds + '(HH:MM:SS)')
    })

    // Ping the NodeJS server
    socket.emit('init')
    errorTimer = setTimeout(() => {
        $('#error-header').text('Web Server Error')
        $('#error-text').text('No response from the NodeJS server. Is it running?')
        $('#error-box').show()
    }, 500)
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
        volume: volume
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
