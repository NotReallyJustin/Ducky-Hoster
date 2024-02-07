/*
    HTTPS log server, but written in express
*/
const express = require("express");
const server = express();

server.use(express.text());

server.all("*", (request, response) => {
    console.log(request.ip);
    console.dir(request.headers);

    console.log(request.body);

    response.end();
});

server.listen(8081, () => {
    console.log("HTTP Log Server (Dev Version) is up.");
});