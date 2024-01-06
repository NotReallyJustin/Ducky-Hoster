/*
    This just contains utility functions the servers might need later down the line.
*/

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

//console.log(this.genAsciiStr(128));