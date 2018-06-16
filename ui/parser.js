const packageHandler = require('./package-handler')

let pkg_start = false // Check whether a package start has received
let len = -1 // Check package length
let buffer = [] // Buffer for incoming package

const parse = ch => {
  if(!pkg_start && ch == 0xAA) {
    //handle head
    pkg_start = true
    len = 0
    buffer.push(ch)
  } else if (pkg_start && len == 0) {
    //handle length
    len = ch
    buffer.push(ch)
  } else if (pkg_start && buffer.length < len - 1) {
    //handle body
    buffer.push(ch)
  } else if (pkg_start && buffer.length == len - 1 && ch == 0xFF) {
    //handle tail
    buffer.push(ch)
    packageHandler(buffer)
    pkg_start = false;
    buffer = [];
  } else {
    //dump
    pkg_start = false // remove the package from handling
    pkg = []
  }
}

module.exports = data => {
  data.forEach(element => {
    parse(element)
  });
}
