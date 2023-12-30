const express = require('express');

// This router will handle the GET requests that ask to retrieve a specified malware
const get_req_router = express.Router({
    mergeParams: true
});



module.exports = get_req_router;