const fs = require('fs');


let buf = fs.readFileSync('../index.html');

let base64 = buf.toString('base64');
let size = 128;
let content ="#define INDEX_HTML "

let off = 0;
while(off <= base64.length) {
    // if(off + size > base64.length) {
    //     break;
    // }
    content += '"' + base64.substring(off, off + size) + '" \\\n';
    off += size;
}
// content += '"' + base64.substring(off, off + size) + '" \\\n';
fs.writeFileSync("./index.h", content, 'utf-8');




