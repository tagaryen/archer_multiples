const fs = require('fs');


let buf = fs.readFileSync('../index.html');

let base64 = buf.toString('base64');
let size = 256;
let content ="const officeJs = "

let off = 0;
while(off <= base64.length) {
    content += '"' + base64.substring(off, off + size) + '" + \n';
    off += size;
}
fs.writeFileSync("./tmp.js", content, 'utf-8');