/*
    This just contains utility functions the servers might need later down the line.
*/

/**
 * Generates a random ASCII string with a specified length
 * @param {Number} length Length of ASCII string to generate
 * @returns {String} Intended ASCII string
 */
module.exports.genAsciiStr = (length) => {
    // Only 32 (32 + 0) - 126 (32 + 94) are valid to print out

    let print_str = "";

    for (i = 0; i < length; i++)
    {
        print_str += String.fromCharCode(32 + Math.floor(Math.random() * 95));
    }

    return print_str;
}