const handleMarkdownHtml = require("./markdown");
const handleImageHtml = require("./img");
const handleTextHtml = require("./text");
const handleDocxHtml = require("./docx");
const handleXlsxHtml = require("./xlsx");

const fs = require('fs');
const html = require('./html');

const contentTypes = {
  '.ai': 'application/postscript',
  '.js': 'application/ecmascript',
  '.edi': 'application/EDI-X12',
  '.json': 'application/json',
  '.ogg': 'application/ogg',
  '.woff': 'application/font-woff',
  '.xhtml': 'application/xhtml+xml',
  '.xml': 'application/xml',
  '.zip': 'application/zip',
  '.gzip': 'application/gzip',
  '.bmp': 'application/x-bmp',
  '.cer': 'application/x-x509-ca-cert',
  '.crt': 'application/x-x509-ca-cert',
  '.der': 'application/x-x509-ca-cert',
  '.pdf': 'application/pdf',
  '.xls': 'application/vnd.ms-excel',
  '.ppt': 'application/vnd.ms-powerpoint',
  '.doc': 'application/msword',
  '.dot': 'application/msword',
  '.rtf': 'application/msword',
  '.exe': 'application/x-msdownload',
  '.dll': 'application/x-msdownload',
  '.apk': 'application/vnd.android.package-archive',
  '.ipa': 'application/vnd.iphone',
  '.torrent': 'application/x-bittorrent',
  '.swf': 'application/x-shockwave-flash',
  '.wmf': 'application/x-wmf',
  // text
  '.323': 'text/h323',
  '.css': 'text/css',
  '.csv': 'text/csv',
  '.html': 'text/html',
  '.htm': 'text/htm',
  '.htx': 'text/htx',
  '.stm': 'text/html',
  '.jsp': 'text/jsp',
  '.md': 'text/markdown',
  '.txt': 'text/plain',
  '.sol': 'text/plain',
  '.sor': 'text/plain',
  '.vcf': 'text/x-vcard',
  '.wsdl': 'text/xml',
  '.xsd': 'text/xsd',
  '.xsl': 'text/xsl',
  '.xslt': 'text/xslt',
  // images
  '.tif': 'image/tiff',
  '.tiff': 'image/tiff',
  '.fax': 'image/fax',
  '.gif': 'image/gif',
  '.ico': 'image/x-icon',
  '.jfif': 'image/jpeg',
  '.jpe': 'image/jpeg',
  '.jpeg': 'image/jpeg',
  '.jpg': 'image/jpeg',
  '.png': 'image/png',
  '.svg': 'image/svg+xml',
  '.wbmp': 'image/vnd.wap.wbmp',
  '.net': 'image/pnetvue',
  '.rp': 'image/vnd.rn-realpix',
  // audio / video
  '.wav': 'audio/wav',
  '.mp3': 'audio/mpeg',
  '.mp4': 'video/mp4',
  '.mpeg': 'video/mpeg',
  '.mpg': 'video/mpeg',
  '.avi': 'video/x-msvideo',
  '.mov': 'video/quicktime',
  '.webm': 'video/webm',
  '.mkv': 'video/mkv',
};

function get404Html() {
    let notFound = html.notFound;
    notFound = notFound.replace('$(icon_template)', "ico");
    return Buffer.from(notFound);
}
function getContentType(name) {
    let dot = name.lastIndexOf('.');
    if(dot < 0) {
        return "text/plain";
    }
    let tail = name.substring(dot);
    let contentType = contentTypes[tail];
    if(!contentType) {
        contentType = "text/plain";
    }
    return contentType;
}

/**
 * @param {ServerResponse} res
*/
function handleStaticDirectory(res, headers, dir) {
    let index = dir + "/index.html";
    headers["Content-Type"] = "text/html";
    if(!fs.existsSync(index)) {
        res.writeHead(200, headers);
        res.end(get404Html());
        return ;
    }
    res.writeHead(200, headers);
    res.end(fs.readFileSync(index));
}

/**
 * @param {ServerResponse} res
*/
function handleStaticFile(res, headers, file, fileName, icon) {
    let suffixIdx = fileName.lastIndexOf(".");
    if(fileName.startsWith('/')) {
        fileName = fileName.substring(1);
    }
    if(suffixIdx > 0) {
        let suffix = fileName.substring(suffixIdx);
        if(suffix === ".docx") {
            headers["Content-Type"] = "text/html";
            res.writeHead(200, headers);
            res.end(Buffer.from(handleDocxHtml(fileName, icon, fs.readFileSync(file, "base64"))));
            return ;
        }
        if(suffix === ".xlsx") {
            headers["Content-Type"] = "text/html";
            res.writeHead(200, headers);
            res.end(Buffer.from(handleXlsxHtml(fileName, icon, fs.readFileSync(file, "base64"))));
            return ;
        }
    }

    let contentType = getContentType(fileName);
    if(contentType === "text/markdown") {
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(Buffer.from(handleMarkdownHtml(fileName, icon, fs.readFileSync(file, "utf-8"))));
        return ;
    }
    if(contentType.startsWith("image/")) {
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(Buffer.from(handleImageHtml(fileName, icon, contentType, fs.readFileSync(file, "base64"))));
        return ;
    }
    if(contentType === "text/plain") {
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(Buffer.from(handleTextHtml(fileName, icon, fs.readFileSync(file, "utf-8"))));
        return ;
    }
    headers["Content-Type"] = contentType;
    res.writeHead(200, headers);
    fs.createReadStream(file).pipe(res);
}

module.exports = {
    handleStaticDirectory,
    handleStaticFile,
    contentTypes,
    getContentType
};
