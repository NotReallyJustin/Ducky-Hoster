const express = require('express');
const EXE_PATHS = require("./exe_paths.js");

// This router will handle the GET requests that ask to retrieve a specified malware.
// The routing is very simple here since GET requests simply act as a CDN for our malware packages
const get_req_router = express.Router({
    mergeParams: true
});

get_req_router.get("/", (request, response) => {
//force auth
//req param?
});

module.exports = get_req_router;