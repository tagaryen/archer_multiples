const handleMarkdownHtml = require("./markdown");
const handleImageHtml = require("./img");
const handleTextHtml = require("./text");
const handleDocxHtml = require("./docx");
const handleXlsxHtml = require("./xlsx");

const fs = require('fs');
const html = require('./html');

const contentTypes = {
    ".pdf": "application/pdf",
    ".ai": "application/postscript",
    ".xml": "application/atom+xml",
    ".js": "application/ecmascript",
    ".edi": "application/EDI-X12",
    ".edi": "application/EDIFACT",
    ".json": "application/json",
    ".js": "application/javascript",
    ".ogg": "application/ogg",
    ".rdf": "application/rdf+xml",
    ".xml": "application/rss+xml",
    ".xml": "application/soap+xml",
    ".woff": "application/font-woff",
    ".xhtml": "application/xhtml+xml",
    ".xml": "application/xml",
    ".dtd": "application/xml-dtd",
    ".xml": "application/xop+xml",
    ".zip": "application/zip",
    ".gzip": "application/gzip",
    ".xls": "application/x-xls",
    ".001": "application/x-001",
    ".301": "application/x-301",
    ".906": "application/x-906",
    ".a11": "application/x-a11",
    ".awf": "application/vnd.adobe.workflow",
    ".bmp": "application/x-bmp",
    ".c4t": "application/x-c4t",
    ".cal": "application/x-cals",
    ".cdf": "application/x-netcdf",
    ".cel": "application/x-cel",
    ".cg4": "application/x-g4",
    ".cit": "application/x-cit",
    ".bot": "application/x-bot",
    ".c90": "application/x-c90",
    ".cat": "application/vnd.ms-pki.seccat",
    ".cdr": "application/x-cdr",
    ".cer": "application/x-x509-ca-cert",
    ".cgm": "application/x-cgm",
    ".cmx": "application/x-cmx",
    ".crl": "application/pkix-crl",
    ".csi": "application/x-csi",
    ".cut": "application/x-cut",
    ".dbm": "application/x-dbm",
    ".cmp": "application/x-cmp",
    ".cot": "application/x-cot",
    ".crt": "application/x-x509-ca-cert",
    ".dbf": "application/x-dbf",
    ".dbx": "application/x-dbx",
    ".dcx": "application/x-dcx",
    ".dgn": "application/x-dgn",
    ".dll": "application/x-msdownload",
    ".dot": "application/msword",
    ".der": "application/x-x509-ca-cert",
    ".dib": "application/x-dib",
    ".doc": "application/msword",
    ".drw": "application/x-drw",
    ".dwf": "application/x-dwf",
    ".dxb": "application/x-dxb",
    ".edn": "application/vnd.adobe.edn",
    ".dwg": "application/x-dwg",
    ".dxf": "application/x-dxf",
    ".emf": "application/x-emf",
    ".epi": "application/x-epi",
    ".eps": "application/postscript",
    ".exe": "application/x-msdownload",
    ".fdf": "application/vnd.fdf",
    ".eps": "application/x-ps",
    ".etd": "application/x-ebx",
    ".fif": "application/fractals",
    ".frm": "application/x-frm",
    ".gbr": "application/x-gbr",
    ".g4": "application/x-g4",
    ".gl2": "application/x-gl2",
    ".hgl": "application/x-hgl",
    ".hpg": "application/x-hpgl",
    ".hqx": "application/mac-binhex40",
    ".hta": "application/hta",
    ".gp4": "application/x-gp4",
    ".hmr": "application/x-hmr",
    ".hpl": "application/x-hpl",
    ".hrf": "application/x-hrf",
    ".icb": "application/x-icb",
    ".ico": "application/x-ico",
    ".ig4": "application/x-g4",
    ".iii": "application/x-iphone",
    ".ins": "application/x-internet-signup",
    ".iff": "application/x-iff",
    ".igs": "application/x-igs",
    ".img": "application/x-img",
    ".isp": "application/x-internet-signup",
    ".jpe": "application/x-jpe",
    ".js": "application/x-javascript",
    ".jpg": "application/x-jpg",
    ".lar": "application/x-laplayer-reg",
    ".latex": "application/x-latex",
    ".lbm": "application/x-lbm",
    ".ls": "application/x-javascript",
    ".ltr": "application/x-ltr",
    ".man": "application/x-troff-man",
    ".mdb": "application/msaccess",
    ".mac": "application/x-mac",
    ".mdb": "application/x-mdb",
    ".mfp": "application/x-shockwave-flash",
    ".mi": "application/x-mi",
    ".mil": "application/x-mil",
    ".mocha": "application/x-javascript",
    ".mpd": "application/vnd.ms-project",
    ".mpp": "application/vnd.ms-project",
    ".mpt": "application/vnd.ms-project",
    ".mpw": "application/vnd.ms-project",
    ".mpx": "application/vnd.ms-project",
    ".mxp": "application/x-mmxp",
    ".nrf": "application/x-nrf",
    ".out": "application/x-out",
    ".p12": "application/x-pkcs12",
    ".p7c": "application/pkcs7-mime",
    ".p7r": "application/x-pkcs7-certreqresp",
    ".pc5": "application/x-pc5",
    ".pcl": "application/x-pcl",
    ".pdx": "application/vnd.adobe.pdx",
    ".pgl": "application/x-pgl",
    ".pko": "application/vnd.ms-pki.pko",
    ".p10": "application/pkcs10",
    ".p7b": "application/x-pkcs7-certificates",
    ".p7m": "application/pkcs7-mime",
    ".p7s": "application/pkcs7-signature",
    ".pci": "application/x-pci",
    ".pcx": "application/x-pcx",
    ".pdf": "application/pdf",
    ".pfx": "application/x-pkcs12",
    ".pic": "application/x-pic",
    ".pl": "application/x-perl",
    ".plt": "application/x-plt",
    ".png": "application/x-png",
    ".ppa": "application/vnd.ms-powerpoint",
    ".pps": "application/vnd.ms-powerpoint",
    ".ppt": "application/x-ppt",
    ".prf": "application/pics-rules",
    ".prt": "application/x-prt",
    ".ps": "application/postscript",
    ".pwz": "application/vnd.ms-powerpoint",
    ".ras": "application/x-ras",
    ".pot": "application/vnd.ms-powerpoint",
    ".ppm": "application/x-ppm",
    ".ppt": "application/vnd.ms-powerpoint",
    ".pr": "application/x-pr",
    ".prn": "application/x-prn",
    ".ps": "application/x-ps",
    ".ptn": "application/x-ptn",
    ".red": "application/x-red",
    ".rjs": "application/vnd.rn-realsystem-rjs",
    ".rlc": "application/x-rlc",
    ".rm": "application/vnd.rn-realmedia",
    ".rat": "application/rat-file",
    ".rec": "application/vnd.rn-recording",
    ".rgb": "application/x-rgb",
    ".rjt": "application/vnd.rn-realsystem-rjt",
    ".rle": "application/x-rle",
    ".rmf": "application/vnd.adobe.rmf",
    ".rmj": "application/vnd.rn-realsystem-rmj",
    ".rmp": "application/vnd.rn-rn_music_package",
    ".rmvb": "application/vnd.rn-realmedia-vbr",
    ".rnx": "application/vnd.rn-realplayer",
    ".rms": "application/vnd.rn-realmedia-secure",
    ".rmx": "application/vnd.rn-realsystem-rmx",
    ".rsml": "application/vnd.rn-rsml",
    ".rtf": "application/msword",
    ".sat": "application/x-sat",
    ".sdw": "application/x-sdw",
    ".slb": "application/x-slb",
    ".rtf": "application/x-rtf",
    ".sam": "application/x-sam",
    ".sdp": "application/sdp",
    ".sit": "application/x-stuffit",
    ".sld": "application/x-sld",
    ".smi": "application/smil",
    ".smk": "application/x-smk",
    ".smil": "application/smil",
    ".spc": "application/x-pkcs7-certificates",
    ".spl": "application/futuresplash",
    ".ssm": "application/streamingmedia",
    ".stl": "application/vnd.ms-pki.stl",
    ".sst": "application/vnd.ms-pki.certstore",
    ".tdf": "application/x-tdf",
    ".tga": "application/x-tga",
    ".sty": "application/x-sty",
    ".swf": "application/x-shockwave-flash",
    ".tg4": "application/x-tg4",
    ".tif": "application/x-tif",
    ".vdx": "application/vnd.visio",
    ".vpg": "application/x-vpeg005",
    ".vsd": "application/x-vsd",
    ".vst": "application/vnd.visio",
    ".vsw": "application/vnd.visio",
    ".vtx": "application/vnd.visio",
    ".torrent": "application/x-bittorrent",
    ".vda": "application/x-vda",
    ".vsd": "application/vnd.visio",
    ".vss": "application/vnd.visio",
    ".vst": "application/x-vst",
    ".vsx": "application/vnd.visio",
    ".wb1": "application/x-wb1",
    ".wb3": "application/x-wb3",
    ".wiz": "application/msword",
    ".wk4": "application/x-wk4",
    ".wks": "application/x-wks",
    ".wb2": "application/x-wb2",
    ".wk3": "application/x-wk3",
    ".wkq": "application/x-wkq",
    ".wmf": "application/x-wmf",
    ".wmd": "application/x-ms-wmd",
    ".wp6": "application/x-wp6",
    ".wpg": "application/x-wpg",
    ".wq1": "application/x-wq1",
    ".wri": "application/x-wri",
    ".ws": "application/x-ws",
    ".wmz": "application/x-ms-wmz",
    ".wpd": "application/x-wpd",
    ".wpl": "application/vnd.ms-wpl",
    ".wr1": "application/x-wr1",
    ".wrk": "application/x-wrk",
    ".ws2": "application/x-ws",
    ".xdp": "application/vnd.adobe.xdp",
    ".xfd": "application/vnd.adobe.xfd",
    ".xfdf": "application/vnd.adobe.xfdf",
    ".xls": "application/vnd.ms-excel",
    ".xwd": "application/x-xwd",
    ".sis": "application/vnd.symbian.install",
    ".x_t": "application/x-x_t",
    ".apk": "application/vnd.android.package-archive",
    ".x_b": "application/x-x_b",
    ".sisx": "application/vnd.symbian.install",
    ".ipa": "application/vnd.iphone",
    ".xap": "application/x-silverlight-app",
    ".xlw": "application/x-xlw",
    ".anv": "application/x-anv",
    ".uin": "application/x-icq",
    ".323": "text/h323",
    ".biz": "text/xml",
    ".cml": "text/xml",
    ".asa": "text/asa",
    ".asp": "text/asp",
    ".css": "text/css",
    ".csv": "text/csv",
    ".dcd": "text/dcd",
    ".dtd": "text/dtd",
    ".ent": "text/ent",
    ".fo": "text/fo",
    ".htc": "text/x-component",
    ".html": "text/html",
    ".htx": "text/htx",
    ".htm": "text/htm",
    ".htt": "text/webviewhtml",
    ".jsp": "text/jsp",
    ".math": "text/xml",
    ".md": "text/markdown",
    ".mml": "text/mml",
    ".mtx": "text/xml",
    ".plg": "text/plg",
    ".rdf": "text/xml",
    ".rt": "text/vnd.rn-realtext",
    ".sol": "text/plain",
    ".spp": "text/xml",
    ".stm": "text/html",
    ".tld": "text/xml",
    ".txt": "text/plain",
    ".uls": "text/iuls",
    ".vml": "text/xml",
    ".tsd": "text/xml",
    ".vcf": "text/x-vcard",
    ".vxml": "text/xml",
    ".wml": "text/vnd.wap.wml",
    ".wsdl": "text/xml",
    ".wsc": "text/scriptlet",
    ".xdr": "text/xdr",
    ".xql": "text/xql",
    ".xsd": "text/xsd",
    ".xslt": "text/xslt",
    ".xml": "text/xml",
    ".xq": "text/xq",
    ".xquery": "text/xquery",
    ".xsl": "text/xsl",
    ".xhtml": "text/xhtml",
    ".odc": "text/x-ms-odc",
    ".r3t": "text/vnd.rn-realtext3d",
    ".sor": "text/plain",
    ".tif": "image/tiff",
    ".fax": "image/fax",
    ".gif": "image/gif",
    ".ico": "image/x-icon",
    ".jfif": "image/jpeg",
    ".jpe": "image/jpeg",
    ".jpeg": "image/jpeg",
    ".jpg": "image/jpeg",
    ".net": "image/pnetvue",
    ".png": "image/png",
    ".rp": "image/vnd.rn-realpix",
    ".svg": "image/svg+xml",
    ".tif": "image/tiff",
    ".tiff": "image/tiff",
    ".wbmp": "image/wbmp",
    ".webp": "image/webp"
}
const hexToInt = {
    '0':0,'1':1,'2':2,'3':3,'4':4,'5':5,'6':6,'7':7,'8':8,'9':9,
    'a':10,'b':11,'c':12,'d':13,'e':14,'f':15,
    'A':10,'B':11,'C':12,'D':13,'E':14,'F':15
};

let referMap = {};

function hexTobytes(hex) {
    if(hex.length >= 2 && hex[0] === '0' && hex[1] === 'x') {
        hex = hex.substring(2);
    }
    if(hex.length & 1 == 1)
        hex = '0'+hex;
    let ret = [];
    for(let i = 0; i < hex.length; i += 2) {
        let b0 = hexToInt[hex[i]];
        let b1 = hexToInt[hex[i+1]];
        ret.push((b0<<4)|b1);
    }
    return ret;
}
function getName(hexName) {
    let idx = hexName.indexOf(".");
    idx = (idx < 0)?hexName.length:idx;
    let bytes = hexTobytes(hexName.substring(0, idx));
    return Buffer.from(bytes).toString("utf-8") + hexName.substring(idx);
}
function getHex(fileName) {
    let idx = fileName.lastIndexOf(".");
    idx = (idx < 0)?fileName.length:idx;
    return Buffer.from(fileName.substring(0, idx), 'utf-8').toString("hex") + fileName.substring(idx);
}
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
function handleReferer(res, headers, referer, url, local) {
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
        file = local + "/" + referMap[referer] + "/" + rawUrl;
    } else {
        let ref = referer.substring(referer.lastIndexOf("/") + 1);
        root = local + "/" + ref;
        if(fs.existsSync(root) && fs.statSync(root).isDirectory()) {
            file = local + "/" + ref + "/" + rawUrl;
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
        let contentType = getContentType(fileName);
        if(!contentType) {
            contentType = "application/octet-stream";
            headers["Content-Disposition", "attachment; filename=\"" + encodeURI(getName(fileName)) + "\""];
        }
        headers["Content-Type"] = contentType;
        res.writeHead(200, headers);
        fs.createReadStream(file).pipe(res);
    } else {
        console.log(`Referer -path:'${file}' Not Found`);
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(get404Html());
    }
}
/** 
 * @param {ServerResponse} res
*/
function handleStaticDirectory(res, headers, root, hexFileName) {
    let index = root + '/' + hexFileName + "/index.html";
    headers["Content-Type"] = "text/html";
    if(!fs.existsSync(index)) {
        console.log(`Index -path:'${index}' Not Found`);
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
function handleStaticFile(res, headers, root, hexFileName, icon) {
    let file = root + '/' + hexFileName;
    let contentType = getContentType(hexFileName);
    if(fs.statSync(file).size > 16 * 1024 * 1024) {  //大于16MB时不提供在线预览能力
        headers["Content-Disposition"] = "attachment; filename=\"" + encodeURI(getName(hexFileName)) + "\""; 
        headers["Content-Type"] = "application/octet-stream";
        res.writeHead(200, headers);
        fs.createReadStream(file).pipe(res);
        return ;
    }
    let suffixIdx = hexFileName.lastIndexOf(".");
    if(suffixIdx > 0) {
        let suffix = hexFileName.substring(suffixIdx);
        if(suffix === ".docx") {
            headers["Content-Type"] = "text/html";
            res.writeHead(200, headers);
            res.end(Buffer.from(handleDocxHtml(getName(hexFileName), icon, fs.readFileSync(file, "base64"))));
            return ;
        }
        if(suffix === ".xlsx") {
            headers["Content-Type"] = "text/html";
            res.writeHead(200, headers);
            res.end(Buffer.from(handleXlsxHtml(getName(hexFileName), icon, fs.readFileSync(file, "base64"))));
            return ;
        }
    }

    if(contentType === "text/markdown") {
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(Buffer.from(handleMarkdownHtml(getName(hexFileName), icon, fs.readFileSync(file, "utf-8"))));
        return ;
    }
    if(contentType.startsWith("image/")) {
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(Buffer.from(handleImageHtml(getName(hexFileName), icon, contentType, fs.readFileSync(file, "base64"))));
        return ;
    }
    if(contentType === "text/plain") {
        headers["Content-Type"] = "text/html";
        res.writeHead(200, headers);
        res.end(Buffer.from(handleTextHtml(getName(hexFileName), icon, fs.readFileSync(file, "utf-8"))));
        return ;
    }
    headers["Content-Type"] = contentType;
    res.writeHead(200, headers);
    fs.createReadStream(file).pipe(res);
}

module.exports = {
    handleStaticDirectory,
    handleStaticFile,
    handleReferer,
    contentTypes,
    getName,
    getHex,
    getContentType
};
