/*
    This just contains utility functions the servers might need later down the line.
*/

/**
 * Generates a random ASCII string with a specified length
 * @param {Number} length Length of ASCII string to generate
 * @returns {String} Intended ASCII string
 */
module.exports.genAsciiStr = (length) => {
    // Only 32 to 126 are valid to print out

    let print_str = "";

    for (var i = 0; i < length; i++)
    {
        print_str = print_str + String.fromCharCode(this.random(32, 126));
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

console.log(this.genAsciiStr(128));