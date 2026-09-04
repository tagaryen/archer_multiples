const http = require('http');
const fs = require('fs');
const os = require('os');
const path = require('path');

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
*/
async function httpHandler(req, res) {
    let headers = {};
    try {
        let url = decodeURIComponent(req.url);
        const filePath = path.join(__dirname, url);
        if(fs.existsSync(filePath)) {
            if(url.endsWith('.js')) {
                type = "text/javascript";
            } else if(url.endsWith('.html')) {
                type = "text/html";
            }
            headers["Content-Type"] = type;
            res.writeHead(200, headers);
            fs.createReadStream(filePath).pipe(res);
        } else {
            headers["Content-Type"] = "text/plain";
            res.writeHead(200, headers);
            res.end("404 not found");
        }
    } catch(e) {
        console.error("Error: ", e)
        headers["Content-Type"] = "text/plain";
        res.writeHead(200, headers);
        res.end("404 not found");
    }
}

const serverPort = 9111;
const server = http.createServer(httpHandler);
server.listen(serverPort);
server.on('error',function(error){
    console.error(error);
});
server.on('listening',function(){
    const interfaces = os.networkInterfaces();
    for (const name of Object.keys(interfaces)) {
        for (const iface of interfaces[name]) {
            if (iface.family === 'IPv4' && !iface.internal) {
                console.log(`访问地址: http://${iface.address}:${serverPort}/`)
                break;
            }
        }
    }
});
