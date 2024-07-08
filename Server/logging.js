/*
    This module handles all the "event" logs. 
    It's not going to be Heroku levels of sophistication, but having these logs will give us a pretty good idea of all the network traffic we're getting.
    All time is documented in UTC.
*/
const path = require("path");
const fs = require("fs");
const { getTimestamp } = require("./util.js");


// Declare path of log files
TRACE_PATH = path.resolve(__dirname, "./Logs/trace");
INFO_PATH = path.resolve(__dirname, "./Logs/info");             // To do: traffic info
STDOUT_PATH = path.resolve(__dirname, "./Logs/stdout");
STDERR_PATH = path.resolve(__dirname, "./Logs/stderr");

/**
 * Logs a given information in `./Logs/trace`.
 * The trace file should give you a general idea of what's happening on the network
 * @param {String} text Text to log
 * @param {Boolean} print2Console Optional parameter - denotes whether you also want to console.log() the instance.
 */
module.exports.log = function(text, print2Console) {

    try
    {
        fs.appendFileSync(TRACE_PATH, `${getTimestamp()} -\t${text}\n`, {encoding: "utf-8"});
        fs.appendFileSync(STDOUT_PATH, `${getTimestamp()} -\t${text}\n`, {encoding: "utf-8"});
    }
    catch(err)
    {
        console.error(`🚨 Logging to trace path with text ${text} failed. Error: ${err}`);
    }

    if (print2Console)
    {
        console.log(text);
    }
}

module.exports.error = function(text) {
    try
    {
        fs.appendFileSync(TRACE_PATH, `${getTimestamp()} -\t${text}\n`, {encoding: "utf-8"});
        fs.appendFileSync(STDERR_PATH, `${getTimestamp()} -\t${text}\n`, {encoding: "utf-8"});
    }
    catch(err)
    {
        console.error(`🚨 Sending error to trace path with text ${text} failed. Error: ${err}`);
    }
}