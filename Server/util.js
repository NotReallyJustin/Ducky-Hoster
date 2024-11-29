/*
    This just contains utility functions the servers might need later down the line.
*/

/**
 * An array of file type extensions that keep their contents as text data.
 * This is used to determine whether we write binary to a file, or write UTF-8 to a file.
 */
module.exports.textExtensions = [
    '.c', '.cpp', '.java', '.py', '.js', '.ts', '.cs', '.swift', '.dta', '.pl', '.sh', '.bat', '.htm', '.html', '.css', 'xhtml', '.asp', '.aspx', '.rss',
    '.txt', '.rtf', '.md', '.csv', '.msg', '.json', '.xml', '.md', '.ps1'
]

/**
 * Generates a random ASCII string with a specified length
 * @param {Number} length Length of ASCII string to generate
 * @returns {String} Intended ASCII string
 */
module.exports.genAsciiStr = (length) => {

    let print_str = "";

    for (var i = 0; i < length; i++)
    {
        print_str = print_str + String.fromCharCode(this.random(48, 126)).replace(/\\/gm, "&");      // @see ASCII table 48-126. \ breaks things so we will ignore it
    }

    return print_str;
}

/**
 * Generates a random integer between low and high
 * @param {Number} low Lower bound
 * @param {Number} high Upper bound
 * @returns {Number} A random integer from within the bounds
 */
module.exports.random = (low, high) => Math.round(Math.random() * (high - low)) + low;

/**
 * @param {String} fileName The file name of a given file 
 * @returns The file extension of a given file
 */
module.exports.getFileExtension = (fileName) => fileName.indexOf(".") == -1 ? "" : fileName.slice(fileName.indexOf("."));

/**
 * Gets the current timestamp (YYYY-MM-DD Hour:Month:Minute:Second)
 * @returns {String} Current timestamp, in UTC and 24 hour format
 */
module.exports.getTimestamp = () => {
    let curr_date = new Date();

    return `${curr_date.getUTCFullYear()}-${curr_date.getUTCMonth() + 1}-${curr_date.getUTCDate()} ${curr_date.getUTCHours()}:${curr_date.getUTCMinutes()}:${curr_date.getUTCSeconds()}`;
}

//console.log(this.genAsciiStr(128));