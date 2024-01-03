const express = require('express');
const EXE_PATHS = require("./exe_paths.js");
const Auth = require("./auth.js");
const { genAsciiStr } = require("./util.js");

// This router will handle the POST requests that are usually used to exfiltrate data from a server
// Basically, exfiltration via HTTP since that's less sus than TCP, and it'd be dumb for firewalls to block Port 80
const postReqRouter = express.Router({
    mergeParams: true
});

// Create a middleware that checks for authentication.
// This is the same setup as get_req.js
postReqRouter.get("*", (request, response, next) => {
    
    let authKey = Auth.postKeys.getWithHash(request.headers.authorization);
    
    if (authKey == null)
    {
        // If unauthorized, send a random 64 bit ASCII
        response.status(401);
        response.send(genAsciiStr(64));
    }
    else
    {
        next();
    }
});

module.exports = postReqRouter;