const http = require('http');
const fs = require('fs');
const path = require('path');

const fileName = 'README.md';
const boundary = '----MyCustomBoundary12345';
const filePath = path.join(__dirname, fileName);

const fileData = fs.readFileSync(filePath);

const bodyStr =
    `--${boundary}\r\n` +
    `Content-Disposition: form-data; name="file"; filename="${fileName}"\r\n` +
    `Content-Type: application/octet-stream\r\n\r\n` +
    fileData +
    `\r\n--${boundary}--\r\n`;

const options = {
    hostname: 'localhost',
    port: 9617,
    path: `/archer/file-api/file-upload?filename=${fileName}`,
    method: 'POST',
    headers: {
        'Content-Type': `multipart/form-data; boundary=${boundary}`,
        'Content-Length': bodyStr.length,
    },
};

const req = http.request(options, (res) => {
    res.on('data', (chunk) => {
        console.log(`response: ${chunk.toString()}`);
    });
});

req.write(bodyStr);
req.end();

// http://localhost:9617/archer/file-api/file-view?filename=README.md
