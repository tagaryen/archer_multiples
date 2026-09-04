const http = require('http');
const fs = require('fs');
const os = require('os');
const url = require('url');
const rootDir = __dirname;
const local = rootDir + "/files";
const staticPath = local + "/static"
const multipartPath = local + "/multipart"
const { exit } = require('process');
const {parseMultipart} = require("./multipart");
const { compressFile,decompressFile } = require("./zip");
const { get404Html, handleStaticDirectory, handleStaticFile, contentTypes, getContentType} = require("./handler");
const {notFound,icon} = require('./html')
const {docxJs, jzipJs, excelJs, xlsxJs, markdJs, hljsCss, highlightJs} = require('./npmjs');

const mobileHtml = Buffer.from(fs.readFileSync(rootDir + "/mobileIndex.html", 'utf-8'));
const indexHtml = Buffer.from(fs.readFileSync(rootDir + "/index.html", 'utf-8'));


let serverPort = 9607;
let referMap = {};
let password = "static^^^pwd";

function parseParam() {
    let argvs = process.argv;
    let port, pwd;
    for(let i = 0; i < argvs.length; i++) {
        if(argvs[i] === '-p') {
            if(i < argvs.length - 1) {
                port = argvs[i+1];
                continue;
            }
        }
        if(argvs[i] === '-pwd') {
            if(i < argvs.length - 1) {
                pwd = argvs[i+1];
                continue;
            }
        }
    }
    if(port) {
        let portNumber = Number(port);
        if(!portNumber) {
            console.error(`invalid param ${port} with -p`);
            exit(0);
        }
        serverPort = portNumber;
    }
    if(pwd) {
        password = pwd;
    }
}

function delFiles(dir) {
    if(!fs.existsSync(dir)) {
        return ;
    }
    if(fs.lstatSync(dir).isDirectory()) {
        for(let f of fs.readdirSync(dir)) {
            delFiles(dir +"/" + f);
        }
        fs.rmdirSync(dir);
    } else {
        fs.unlinkSync(dir);
    }
}
function getIndexHtml(isMobile) {
    if(isMobile) {
        return mobileHtml
    } else {
        return indexHtml
    }
}
/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
*/
function handleNpmjs(req, res, headers, url) {
    let content = null;
    if(url.startsWith("/npmjs/jszip")) {
        content = Buffer.from(jzipJs, 'base64');
    }
    if(url.startsWith("/npmjs/docx")) {
        content = Buffer.from(docxJs, 'base64');
    }
    if(url.startsWith("/npmjs/excel")) {
        content = Buffer.from(excelJs, 'base64');
    }
    if(url.startsWith("/npmjs/xlsx")) {
        content = Buffer.from(xlsxJs, 'base64');
    }
    if(url.startsWith("/npmjs/markd")) {
        content = Buffer.from(markdJs, 'base64');
    }
    if(url.startsWith("/npmjs/highlight")) {
        content = Buffer.from(highlightJs, 'base64');
    }
    headers["Content-Type"] = "application/javascript";
    if(url.startsWith("/npmjs/hljs.css")) {
        headers["Content-Type"] = "text/css";
        content = hljsCss;
    }
    if(content == null) {
        headers["Content-Type"] = "application/json";
        res.writeHead(404, headers);
        res.end('{"success":false, "message":"Can not found npmjs file"}');
        return ;
    }
    res.writeHead(200, headers);
    res.end(content);
}
/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
*/
function handleList(req, res, headers) {
    let dirs = fs.readdirSync(local).filter(f => {return f !== 'static' && f !== 'multipart'});
    let contentList = [];
    for(let f of dirs) {
        let stat = fs.statSync(local + "/" + f);
        let url = f;
        if(url.endsWith('.zip')) {
            url = url.substring(0, url.length - 4);
        }
        let time = stat.birthtime;
        contentList.push({name:f,url, time});
    }
    contentList.sort((o1, o2) => {
        return o2.time - o1.time;
    });
    let resultList = [];
    for(let o of contentList) {
        let time = o.time;
        let month = time.getMonth() + 1;
        if(month < 10) {
            month = "0" + month;
        }
        let date = time.getDate();
        if(date < 10) {
            date = "0" + date;
        }
        let timeStr = time.getFullYear() + "-" + month + "-" + date + " " + time.toLocaleTimeString();
        resultList.push({name: o.name,url: o.url, time: timeStr});
    }
    let body = {success: true, data: {total: contentList.length, pageNum: 1, files: resultList}};
    headers["Content-Type"] = "application/json";
    res.writeHead(200, headers);
    res.end(JSON.stringify(body));
}
/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
*/
function handleDownload(req, res, headers) {
    try {
        let parsedUrl = url.parse(req.url, true);
        let name = parsedUrl.query.filename;
        if(!name) {
            headers["Content-Type"] = "application/json";
            res.writeHead(200, headers);
            res.end('{"success":false, "message":"Invalid filename '+name+'"}');
            return ;
        }
        let filePath = local +"/"+name;
        if(!fs.existsSync(filePath)) {
            headers["Content-Type"] = "application/json";
            res.writeHead(200, headers);
            res.end('{"success":false, "message":"File '+name+' not found"}');
            return ;
        }
        headers["Content-Disposition"] = "attachment; filename=\"" + encodeURI(name) + "\""; 
        headers["Content-Type"] = "application/octet-stream";
        headers["Content-Length"] = '' + fs.statSync(filePath).size;
        res.writeHead(200, headers);
        const readStream = fs.createReadStream(filePath);
        readStream.on('error',(err) => {
            console.error(err);
        });
        readStream.pipe(res);
    } catch(e) {
        console.error("Error ", e);
        headers["Content-Type"] = "application/json";
        res.writeHead(400, headers);
        res.end('{"success":false, "message":"Param error"}');
    }
}
/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
*/
function handleUpload(req, res, headers) {
    // let form = new multiparty.Form({uploadDir: multipartPath});
    parseMultipart(req, multipartPath, (error, fields, files) => {
        let body = {
            success : false,
            message : "上传失败"
        };
        let tmpFile = files.file[0].path;
        let fileName = files.file[0].originalFilename;
        if(error) {
            console.error("上传文件失败,", error)
        } else {
            try {
                if(fileName.length > 256) {
                    console.error("名称长度上限为256，实际长度为", fileName.length)
                    body.message = "名称长度超过限制";
                } else {
                    let filePath = local + "/" + fileName;
                    if(fs.existsSync(filePath)) {
                        body.message = "名称重复";
                    } else {
                        fs.renameSync(tmpFile, filePath);
                        if(fileName.endsWith(".zip")) {
                            let staticDir = staticPath + "/" + fileName.substring(0, fileName.length - 4);
                            fs.mkdirSync(staticDir, {recursive: true});
                            decompressFile(filePath, staticDir);
                            let childs = fs.readdirSync(staticDir);
                            if(childs.length === 1 && fs.statSync(staticDir + "/" + childs[0]).isDirectory()) {
                                fs.readdirSync(staticDir + "/" + childs[0]).map(c => {
                                    fs.renameSync(staticDir + "/" + childs[0] + "/" + c, staticDir + "/" + c);
                                });
                                fs.rmdirSync(staticDir + "/" + childs[0]);
                            }
                        }
                        body.success = true;
                        body.message = "success"
                    }
                }
            } catch(ex) {
                console.error("上传文件失败,", ex)
            }
        }
        if(fs.existsSync(tmpFile)) {
            delFiles(tmpFile);
        }
        headers["Content-Type"] = "application/json";
        res.writeHead(200, headers);
        res.end(JSON.stringify(body));
    });
}
/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
*/
function handleRemove(req, res, headers) {
    let body = ''; 
    req.on('data', chunk => {
        body += chunk;
    });
    req.on('end', () => {
        try {
            let param = JSON.parse(body);
            let name = param.name;
            let pwd = param.password;
            if(password !== pwd) {
                headers["Content-Type"] = "application/json";
                res.writeHead(200, headers);
                res.end('{"success":false, "message":"Password wrong"}');
                return ;
            }
            let filePath = local + "/" + name;
            fs.unlinkSync(filePath);
            if(name.endsWith('.zip')) {
                name = name.substring(0, name.length - 4);
                let staticDir = staticPath + "/" + name;
                delFiles(staticDir);

            }
            headers["Content-Type"] = "application/json";
            res.writeHead(200, headers);
            res.end('{"success":true}');
        } catch(e) {
            console.error("Invalid body ", body);
            headers["Content-Type"] = "application/json";
            res.writeHead(400, headers);
            res.end('{"success":false, "message":"Param error"}');
        }
    });

    req.on('error', err => {
        headers["Content-Type"] = "application/json";
        res.writeHead(503, headers);
        res.end('{"success":false, "message":"Service error"}');
    });
}

function handleReferer(res, headers, referer, url) {
    let rawUrl = url;
    if(rawUrl.startsWith("/")) {
        rawUrl = rawUrl.substring(1, rawUrl.length);
    }
    if(referer.endsWith("/")) {
        referer = referer.substring(0, referer.length - 1);
    }
    let file = null;
    if(referMap[referer]) {
        referMap[rawUrl] = referMap[referer];
        file = staticPath + "/" + referMap[referer] + "/" + rawUrl;
    } else {
        let ref = referer.substring(referer.lastIndexOf("/") + 1);
        let root = staticPath + "/" + ref;
        if(fs.existsSync(root) && fs.statSync(root).isDirectory()) {
            file = root + "/" + rawUrl;
            referMap[rawUrl] = ref;
        }
    }
    if(file && fs.existsSync(file)) {
        let stat = fs.lstatSync(file);
        if(stat.isDirectory()) {
            if(file.endsWith("/")) {
                file += "index.html";
            } else {
                file += "/index.html";
            }
        }
        let fileName = file.substring(file.lastIndexOf("/") + 1, file.length);
        let contentType = getContentType(file);
        if(!contentType) {
            contentType = "application/octet-stream";
            headers["Content-Disposition", fileName];
        }
        headers["Content-Type"] = contentType;
        res.writeHead(200, headers);
        res.end(fs.readFileSync(file));
    } else {
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(get404Html());
    }
}

/**
 * @param {http.IncomingMessage} req
 * @param {http.ServerResponse} res
*/
async function httpHandler(req, res) {
    let url = req.url;
    let method = req.method;
    let userAgent = req.headers['user-agent'];
    let isMobile = (userAgent.indexOf('iPhone OS') > 0) || (userAgent.indexOf('Android') > 0);
    let headers = {};
    try {
        if(!url || url === "/") {
            headers["Content-Type"] = "text/html";
            res.writeHead(200, headers);
            res.end(getIndexHtml(isMobile));
            return ;
        }
        if(method === "GET" && (url === '/ico' || url === '/favicon.ico')) {
            headers["Content-Type"] = "application/x-ico";
            res.writeHead(200, headers);
            res.end(Buffer.from(icon, 'base64'));
            return ;
        }
        if(method === "GET" && url.startsWith("/list?pageNum=")) {
            handleList(req, res, headers);
            return ;
        }
        if(method === "POST" && (url === "remove" || url === "/remove")) {
            handleRemove(req, res, headers);
            return ;
        }
        if(method === "POST" && (url === "upload" || url === "/upload")) {
            handleUpload(req, res, headers);
            return ;
        }
        if(method === "GET" && (url.startsWith("/download?filename="))) {
            await handleDownload(req, res, headers);
            return ;
        }
        if(method === "GET" && (url.startsWith("/npmjs/"))) {
            handleNpmjs(req, res, headers, url);
            return ;
        }
        url = decodeURI(url);
        console.log("Request -url:'" + url + "', -referer = " + req.headers.referer)
        if(req.headers.referer) {
            let referer = decodeURI(req.headers.referer);
            referer = referer.replace(/^https?:\/\/[^\/]+/, '');
            if(referer !== "" && referer !== "/") {
                try {
                    handleReferer(res, headers, referer, url);
                } catch(e) {
                    console.error("handleReferer 错误: ", e)
                    headers["Content-Type"] = "text/html";
                    res.writeHead(200, headers);
                    res.end(get404Html());
                }
                return ;
            }
        }
        
        let file = local  + url;
        if(fs.existsSync(file)) {
            handleStaticFile(res, headers, file, url, "ico");
            return ;
        }
        let dir = staticPath + url;
        if(fs.existsSync(dir)) {
            handleStaticDirectory(res, headers, dir);
            return ;
        }
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(get404Html());
    } catch(e) {
        console.error("Error: ", e)
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(get404Html());
    }
}

parseParam();
fs.mkdirSync(staticPath, {recursive:true});
fs.mkdirSync(multipartPath, {recursive:true});
const server = http.createServer(httpHandler);
server.listen(serverPort);
server.on('error',function(error){
    console.error(error);
});
server.on('listening',function(){
    console.log('文件删除密码: '+password);
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