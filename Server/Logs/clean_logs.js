/*
    Quick node script to clean all the logs in the current directory
*/

const fs = require("fs");

// 🚨 We're hard-coding these logs for security purposes (so someone can't accidentally wipe out more server-side files than intended)
// However, if you do end up adding more logs, change this log_list
const log_list = [
    "./info",
    "./stderr",
    "./stdout",
    "./trace"
];

const remove_logs = log_list.map((log_path) => {
    return fs.writeFile(log_path, "", {
        encoding: 'utf-8'
    });
});

Promise.all(remove_logs)
    .then(() => {
        console.log("Logs cleared.");
    })
    .catch(err => {
        console.error(`Logs failed to clear: ${err}.`);
    });