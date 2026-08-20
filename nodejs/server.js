const http = require('http');
const fs = require('fs');
const url = require('url');
const path = require('path');
const rootDir = process.cwd();
const local = rootDir + "/static";
const { exit } = require('process');
const multiparty = require('multiparty');
const zl = require("zip-lib");
const { handleStaticDirectory, handleStaticFile, handleReferer, contentTypes, getName, getHex} = require("./handler");
const {indexHtml,notFound,icon} = require('./html')
const {docxJs, jzipJs, excelJs, xlsxJs, markdJs, hljsCss, highlightJs} = require('./npmjs');

let serverPort = 9607;
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
            console.log(`invalid param ${port} with -p`);
            exit(0);
        }
        serverPort = portNumber;
    }
    if(pwd) {
        password = pwd;
    }
}
function getFolderSize(dirPath) {
    let totalSize = 0;
    function walk(currentPath) {
        let entries = fs.readdirSync(currentPath, { withFileTypes: true });
        for (let entry of entries) {
            let fullPath = currentPath +"/" + entry.name;
            if (entry.isFile()) {
                let stats = fs.statSync(fullPath);
                totalSize += stats.size;
            } else if (entry.isDirectory()) {
                walk(fullPath);
            }
        }
    }
    walk(dirPath);
    return totalSize;
}
function moveContentsUpSync(sourceDir) {
    let parentDir = path.dirname(sourceDir);
    let entries = fs.readdirSync(sourceDir, { withFileTypes: true });
    for (let entry of entries) {
        let srcPath = path.join(sourceDir, entry.name);
        let destPath = path.join(parentDir, entry.name);
        if (fs.existsSync(destPath)) {
            fs.rmSync(destPath, { recursive: true, force: true });
        }
        fs.renameSync(srcPath, destPath);
    }
    if (fs.readdirSync(sourceDir).length === 0) {
        fs.rmdirSync(sourceDir);
    }
}
function delFiles(dir) {
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
        // return Buffer.from(indexHtml, 'base64');
        return Buffer.from(fs.readFileSync("./mobileIndex.html", 'utf-8'));
    } else {
        // return Buffer.from(indexHtml, 'base64');
        return Buffer.from(fs.readFileSync("./index.html", 'utf-8'));
    }
}
function get404Html() {
    return Buffer.from(notFound, 'utf-8');
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
    let parsedUrl = url.parse(req.url, true);
    let dirs = fs.readdirSync(local);
    let contentList = [];
    for(let f of dirs) {
        let stat = fs.statSync(local + "/" + f);
        let url = f, name = getName(f), size = stat.size;
        if(stat.isDirectory()) {
            name += ".zip"
            size = getFolderSize(local + "/" + f);
        }
        if(!name) {
            delFiles(local + "/" + f);
        } else {
            let time = stat.birthtime;
            contentList.push({name, url, time, size});
        }
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
        resultList.push({name: o.name,url: o.url, time: timeStr, size: o.size});
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
async function handleDownload(req, res, headers) {
    try {
        let parsedUrl = url.parse(req.url, true);
        let name = parsedUrl.query.filename;
        if(!name) {
            headers["Content-Type"] = "application/json";
            res.writeHead(200, headers);
            res.end('{"success":false, "message":"Invalid filename '+name+'"}');
            return ;
        }
        let hex = getHex(name);
        let absFile = local +"/"+hex;
        if(!fs.existsSync(absFile)) {
            headers["Content-Type"] = "application/json";
            res.writeHead(200, headers);
            res.end('{"success":false, "message":"File '+name+' not found"}');
            return ;
        }
        let filePath
        if(fs.statSync(absFile).isDirectory()) {
            filePath = rootDir + "/" + hex + ".zip"
            await zl.archiveFolder(absFile, filePath);
        } else {
            filePath = absFile;
        }
        headers["Content-Disposition"] = "attachment; filename=\"" + encodeURI(name) + "\""; 
        headers["Content-Type"] = "application/octet-stream";
        res.writeHead(200, headers);
        fs.createReadStream(filePath).pipe(res);
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
    let form = new multiparty.Form({uploadDir: local});
    form.parse(req, async (error, fields, files) => {
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
                let hex = getHex(fileName);
                if(hex.length > 256) {
                    console.error("16进制的名称长度上限为256，实际长度为", hex.length)
                    body.message = "名称长度超过限制";
                } else {
                    let absFile = local +"/" + hex;
                    if(fs.existsSync(absFile)) {
                        body.message = "名称重复";
                    } else {
                        if(fileName.endsWith(".zip")) {
                            let dot = absFile.lastIndexOf(".");
                            if(dot > 0) {
                                absFile = absFile.substring(0, dot);
                            }
                            fs.mkdirSync(absFile);
                            await zl.extract(tmpFile, absFile);
                            let childs = fs.readdirSync(absFile);
                            if(childs.length === 1 && fs.statSync(absFile + "/" + childs[0]).isDirectory()) {
                                moveContentsUpSync(absFile + "/" + childs[0]);
                            }
                            // await zipUncompress(tmpFile, absFile);
                        } else {
                            fs.writeFileSync(absFile, fs.readFileSync(tmpFile));
                        }
                        body.success = true;
                        body.message = "success"
                    }
                }
            } catch(ex) {
                console.error("上传文件失败,", ex)
            }
        }
        delFiles(tmpFile);
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
            let hex = getHex(name);
            if(hex.endsWith('.zip')) {
                hex = hex.substring(0, hex.length - 4);
            }
            let absFile = null;
            for(let f of fs.readdirSync(local)) {
                if(f === hex) {
                    absFile = local +"/"+f;
                    break;
                }
            }
            if(absFile) {
                delFiles(absFile);
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
                    handleReferer(res, headers, referer, url, local);
                } catch(e) {
                    console.log("handleReferer 错误: ", e)
                    headers["Content-Type"] = "text/html";
                    res.writeHead(200, headers);
                    res.end(get404Html());
                }
                return ;
            }
        }
        let file = local + url;
        if(!fs.existsSync(file)) {
            console.log(`File -path:'${file}' Not Found`);
            headers["Content-Type"] = "text/html";
            res.writeHead(200, headers);
            res.end(get404Html());
            return ;
        }
        let stat = fs.lstatSync(file);
        if(url[0] === '/') {
            url = url.substring(1, url.length);
        }
        if(stat.isDirectory()) {
            handleStaticDirectory(res, headers, local, url);
        } else {
            handleStaticFile(res, headers, local, url, "ico");
        }
    } catch(e) {
        console.log("Error: ", e)
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(get404Html());
    }
}

parseParam();
if(!fs.existsSync(local)) {
    fs.mkdirSync(local);
}
const server = http.createServer(httpHandler);
server.listen(serverPort);
server.on('error',function(error){
    console.log(error);
});
server.on('listening',function(){
    const address = server.address();
    console.log('Password: '+password);
    console.log('Server listening on '+address.port);
});