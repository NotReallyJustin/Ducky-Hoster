/*
    This HTTP server is a minimal piece of software that's used to test what our POST requests are sending out.
    It's not used in the final version of Ducky Hoster.
    However, it is a pretty versatile dev test tool so I'm leaving this in the repository.
*/

const http = require("http");

const server = http.createServer((request, response) => {
    // Just log the requests we get (and appropriate headers)
    console.dir(request.headers);

    // Print out the content we recieved
    let content = "";
    request.on("readable", () => {
        content += request.read();
    });

    request.on("end", () => {
        console.log(content);
    });

    // Then, end the response
    response.end();
});

server.listen(8081);
console.log("HTTP Log Server (Dev Version) is up.");