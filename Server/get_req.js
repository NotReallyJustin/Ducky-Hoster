const express = require('express');
const EXE_PATHS = require("./exe_paths.js");
const Auth = require("./auth.js");
const { genAsciiStr } = require("./util.js");

// This router will handle the GET requests that ask to retrieve a specified malware.
// The routing is very simple here since GET requests simply act as a CDN for our malware packages
const getReqRouter = express.Router({
    mergeParams: true
});

// Create a middleware that checks for authentication. See auth.js for more info on how that's done
getReqRouter.get("*", (request, response, next) => {
    
    let authKey = Auth.getKeys.getWithHash(request.headers.authorization);
    
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

module.exports = getReqRouter;