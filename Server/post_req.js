const express = require('express');

// This router will handle the POST requests that are usually used to exfiltrate data from a server
// Basically, exfiltration via HTTP since that's less sus than TCP, and it'd be dumb for firewalls to block Port 80
const post_req_router = express.Router({
    mergeParams: true
});



module.exports = post_req_router;