// Code for starting the server. Taken from the DynamicExample code example.
// Note that this is different than the browser based JS that runs the UI of the web page.

"use strict"

const PORT_NUMBER = 8088

var http = require('http')
var fs = require('fs')
var path = require('path')
var mime = require('mime')

var server = http.createServer((req, res) => {
    var filePath = false

    // Squelch favicon requests
    if (req.url == '/favicon.ico') {
        squelchFavicon(res)
        return
    }

    if (req.url == '/') {
        filePath = 'index.html'
    } else {
        filePath = req.url
    }

    var absPath = './' + filePath
    serveStatic(res, absPath)
})

function serveStatic(res, absPath) {
    fs.stat(absPath, (err) => {
        if (err) { // Error locating the file. Send an error 404 message
            console.log(err)
            send404(res)
        } else {
            fs.readFile(absPath, (err, fileContents) => { // Error accessing the file. Send an error 404 message
                if (err) {
                    console.log(err)
                    send404(res)
                } else {
                    console.log("Sending " + absPath)
                    sendFile(res, absPath, fileContents)
                }
            })
        }
    })
}

// Function for squelching favicon requests
function squelchFavicon(res) {
    console.log("Favicon requested. Squelching.")
}

// Function for sending the error 404 page
function send404(res) {
    res.writeHead(404 , {'Content-Type' : 'text/plain'})
    res.write('Error 404: Resource not found.')
    res.end()
}

// Function for sending the HTML document
function sendFile(res, filePath, fileContents) {
    res.writeHead(200, {"content-type" : mime.lookup(path.basename(filePath))})
    res.end(fileContents)
}

server.listen(PORT_NUMBER, () => {
    console.log("Server listening on port " + PORT_NUMBER)
})

const UDP_PORT = 8089
var beatboxServer = require('./lib/beatbox_server')
beatboxServer.listen(server, UDP_PORT)
