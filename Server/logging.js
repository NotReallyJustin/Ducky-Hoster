/*
    This module handles all the "event" logs. 
    It's not going to be Heroku levels of sophistication, but having these logs will give us a pretty good idea of all the network traffic we're getting.
    All time is documented in UTC.
*/
const path = require("path");
const fs = require("fs");


// Declare path of log files
TRACE_PATH = path.resolve(__dirname, "./Logs/trace");
INFO_PATH = path.resolve(__dirname, "./Logs/info");             // To do: traffic info
STDOUT_PATH = path.resolve(__dirname, "./Logs/stdout");
STDERR_PATH = path.resolve(__dirname, "./Logs/stderr");

/**
 * Gets the current date and time in `YYYY/MM/DD 24HR:Minute:Second format`
 */
const getDateTime = () => {
    let curr_date = new Date();

    return `${curr_date.getUTCFullYear()}/${curr_date.getUTCMonth() + 1}/${curr_date.getUTCDate()} ${curr_date.getUTCHours()}:${curr_date.getUTCMinutes()}:${curr_date.getUTCSeconds()}`;
}

/**
 * Logs a given information in `./Logs/trace`.
 * The trace file should give you a general idea of what's happening on the network
 * @param {String} text Text to log
 * @param {Boolean} print2Console Optional parameter - denotes whether you also want to console.log() the instance.
 */
module.exports.log = function(text, print2Console) {
    fs.appendFile(TRACE_PATH, `${getDateTime()} -\t${text}`, {encoding: "utf-8"}, (err) => {
        if (err)
        {
            console.error(`🚨 Logging to trace path with text ${text} failed. Error: ${err}`);
        }
        else
        {
            if (print2Console)
            {
                console.log(text);
            }
        }
    })
}

module.exports.error = function(text) {
    fs.appendFile(TRACE_PATH, `${getDateTime()} -\t${text}`, {encoding: "utf-8"}, (err) => {
        if (err)
        {
            console.error(`🚨 Logging to trace path with text ${text} failed. Error: ${err}`);
        }
        else
        {
            console.error(text);
        }
    });
}