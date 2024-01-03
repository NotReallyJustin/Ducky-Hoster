const path = require("path");

/**
 * Creates an EXEData object that will store all the necessary data that corresponds to a requested EXE string provided by the HTTP EXE parameter.
 * ELI5 definition: when the user requests a certain EXE, this class contains the data for said EXE.
 * @param {String} cPath Path to the C file (that we will compile upon request). This should be relative.
 * @param {Boolean} usesAuthKey Whether this EXE requires an POST request AuthKey to be generated (usually for exfiltration)
 * @param {Boolean} singleUse Whether the auth key should be single use
 */
const EXEData = function(cPath, usesAuthKey, singleUse) {

    /**
     * Absolute path to the EXE file
     * @type {String}
     */
    this.cPath = path.resolve(__dirname, cPath);

    /**
     * Whether this EXE requires an POST request AuthKey to be generated (usually for exfiltration)
     * @type {Boolean}
     */
    this.usesAuthKey = usesAuthKey;

    /**
     * If this EXE requires an auth key, this determines whether or not it's single use
     * @type {Boolean}
     */
    this.singleUse = singleUse || false;
}

/**
 * A JSON that maps EXE parameters we recieve from the Router to server-side paths.
 * Our server will then compile the C file in the server-side path, and then return it.
 * Not the most efficient way of doing things, but this is the most secure way of allowing user file retrieval
 */
module.exports = {
    a:1
};