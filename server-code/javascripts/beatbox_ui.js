// Code for driving client-based interactions on the index.html webpage.
console.log("Hi from beatbox_ui.js!")

// this is the client-side version of socket.io
const socket = io.connect()

var errorTimer

// Setup for websocket pakcet listeners
$(document).ready(() => {
    $('#volumeid').focus();
    $('#status').text('Connecting...')

    // Set up the listeners for message types
    socket.on('noConnect', () => {
        // Display message if there's no connection to Beatbox C app
        $('#error-header').text('Beatbox Error')
        $('#error-text').text('No response from Beatbox! Is it running?')
        $('#error-box').show()
    })

    socket.on('jsAck', () => {
        // Display message for successful ping to JS server
        clearTimeout(errorTimer)
    })

    socket.on('uptime', (msg) => {
        clearTimeout(errorTimer)
        // console.log('DEBUG: Received seconds: ' + msg.reply)
        let seconds = Number(msg.reply)
        let minutes = Math.trunc(seconds / 60)
        seconds %= 60
        let hours = Math.trunc(minutes / 60)
        minutes %= 60

        $('#error-box').hide()
        $('#status').text('Beatbox\'s device has been running for ' + hours + ':' + minutes + ':' + seconds + '(HH:MM:SS)')
    })

    socket.on('mode', (msg) => {
        clearNodejsErrorTimer()
        // console.log('DEBUG: Received mode: ' + msg.reply)
        let currentMode = msg.mode
        let reply = msg.reply

        $('#modeid').text(currentMode)
        $('#msg-text').append('<br />' + reply)
    })

    socket.on('volume', (msg) => {
        clearNodejsErrorTimer()
        let reply = msg.reply
        if (msg.status == 't') { // successful volume change
            $('#volumeid').text(msg.vol)
            $('#msg-text').append('<br />' + reply)
        }
        else if (msg.status == 'f'){
            $('#msg-text').append('<br />' + reply)
        }
        else {
            console.log('DEBUG: something unexpected happened where status for volume is ' + msg.status)
        }
    })

    socket.on('tempo', (msg) => {
        clearNodejsErrorTimer()
        let reply = msg.reply
        if (msg.status == 't') {
            $('#tempoid').text(msg.tempo + ' BPM')
            $('#msg-text').append('<br />' + reply)
        }
        else if (msg.status == 'f') {
            $('#msg-text').append('<br />' + reply)
        }
        else {
            console.log('DEBUG: something unexpected happened where status for tempo is ' + msg.status)
        }
    })

    socket.on('sound', (msg) => {
        clearNodejsErrorTimer()
        let reply = msg.reply
        $('#msg-text').append('<br />' + reply)
    })

    socket.on('stop', (msg) => {
        clearNodejsErrorTimer()
        let reply = msg.reply
        $('#msg-text').append('<br />' + reply)
    })

    socket.on('unknown', (msg) => {
        clearNodejsErrorTimer()
        let reply = msg.reply
        $('#msg-text').append('<br />' + reply)
    })

    socket.on('status', (msg) => {
        clearNodejsErrorTimer()
        $('#modeid').text(msg.mode)
        $('#volumeid').text(msg.vol)
        $('#tempoid').text(msg.bpm + ' BPM')
    })

    // Ping the NodeJS server
    socket.emit('init')
    setNodejsErrorTimer()
})

function setNodejsErrorTimer() {
    errorTimer = setTimeout(() => {
        $('#error-header').text('Web Server Error')
        $('#error-text').text('No response from the NodeJS server. Is it running?')
        $('#error-box').show()
    }, 500)
}

function clearNodejsErrorTimer() {
    clearTimeout(errorTimer)
    $('#error-box').hide()
}

// Status setters
$('#modeNone').click(() => {
    var message = {
        mode : 0
    }
    socket.emit('mode', message)
    setNodejsErrorTimer()
})

$('#modeRock1').click(() => {
    var message = {
        mode : 1
    }
    socket.emit('mode', message)
    setNodejsErrorTimer()
})

$('#modeRock2').click(() => {
    var message = {
        mode : 2
    }
    socket.emit('mode', message)
    setNodejsErrorTimer()
})

$('#volumeSet').click(() => {
    let volume = Number($('#volumeTextBox').val())
    var message = {
        volume: volume
    }
    socket.emit('volume', message)
    setNodejsErrorTimer()
})

$('#bpmSet').click(() => {
    let bpm = Number($('#bpmTextBox').val())
    var message = {
        bpm: bpm
    }
    socket.emit('tempo', message)
    setNodejsErrorTimer()
})

$('#playHiHat').click(() => {
    let mode = '0'
    var message = {
        sound: mode
    }
    socket.emit('sound', message)
    setNodejsErrorTimer()
})

$('#playBassDrum').click(() => {
    let mode = '1'
    var message = {
        sound: mode
    }
    socket.emit('sound', message)
    setNodejsErrorTimer()
})

$('#playSnareDrum').click(() => {
    let mode = '2'
    var message = {
        sound: mode
    }
    socket.emit('sound', message)
    setNodejsErrorTimer()
})

$('#statusButton').click(() => {
    socket.emit('status')
    setNodejsErrorTimer()
})

// Everything below is UI related
$('#stopProgram').click(() => {
    socket.emit('stop', 'stop')
    setTimeout(() => { 
        $('#status').text('Shutting down beatbox...')
    }, 300)
})

$('#volumeUp').click(() => {
    let volume = Number($('#volumeTextBox').val())
    if (volume < 100) {
        $('#volumeTextBox').val(volume + 1)
    }
})

$('#volumeDown').click(() => {
    let volume = Number($('#volumeTextBox').val())
    if (volume > 0) {
        $('#volumeTextBox').val(volume - 1)
    }
})

$('#bpmUp').click(() => {
    let volume = Number($('#bpmTextBox').val())
    if (volume < 300) {
        $('#bpmTextBox').val(volume + 1)
    }
})

$('#bpmDown').click(() => {
    let volume = Number($('#bpmTextBox').val())
    if (volume > 40) {
        $('#bpmTextBox').val(volume - 1)
    }
})
