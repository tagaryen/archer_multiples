const fs = require('fs');
const path = require('path');

/**
 * @param {string} url 
 * @param {string} method 
 * @param {Object} headers 
 * @param {any} body 
 * @param {function(http.IncomingMessage, string):void} bodyCallback 
 * 
*/
function request(method, url, headers, body, bodyCallback) {
    const METHODS = {'GET':true, 'POST':true, 'PUT':true, 'DELETE':true, 'OPTION':true}
    if(!METHODS[method]) {
        throw new Error("Method Not Allowed")
    }
    let uri = "/", ssl = false, host = "127.0.0.1", port = 80;
    let l = url;
    if(l.startsWith("https://")) {
        ssl = true;
        l = l.substring(8, url.length)     
    } else if(l.startsWith("http://")) {
        l = l.substring(7, url.length)
    } else {
        throw new Error("Invalid url " + url)
    }
    let i = l.indexOf(':'), j = l.indexOf('/');
    if(j === -1 && i === -1) {
        host = l;
        port = ssl ? 443 : 80;
        l = "";
    } else if(j < i || i === -1) {
        host = l.substring(0, j);
        port = ssl ? 443 : 80;
        l = l.substring(j, l.length);
    } else {
        host = l.substring(0, i);
        port = parseInt(l.substring(i+1, j));
        l = l.substring(j, l.length);
    }
    if(l.length > 0) {
        uri = l;
    }
    const http = require('http');
    const https = require('https');

    let client = ssl ? https: http;
    let hasContentType = false;
    let theHeaders = headers;
    if(theHeaders) {
        for(let k in theHeaders) {
            if(k.toLocaleLowerCase() === 'content-type') {
                hasContentType = true;
                break;
            }
        }
        if(!hasContentType) {
            theHeaders['Content-Type'] = 'application/json';
        }
    } else {
        theHeaders = {'Content-Type': 'application/json'};
    }

    let param = {hostname: host,port: port,path: uri,method: method,headers: theHeaders};

    let req = client.request(param, (res) => {
        let body = '';
        res.on('data', (chunk) => {body += chunk;});
        res.on('end', () => {bodyCallback(res, body)});
    });
    req.on('error', (err) => {console.error(err);})
    if(method !== 'GET' || body) {
        if(body instanceof String) {
            req.write(body);
        } if(body instanceof Buffer) {
            req.write(body);
        } else {
            req.write(JSON.stringify(body));
        }
    }
    req.end();
}

request("POST", "http://127.0.0.1:9607/archer/proxy-api/proxy-add", null, {
    "name":"必应代理配置",
    "requestPath": "/bing",
    "proxyUrl": "https://cn.bing.com/search",
    "requestHeaders": {},
    "responseHeaders":{}
}, (res, body) => {
    console.log("add result: ",body);
});

request("GET", "http://127.0.0.1:9607/archer/proxy-api/proxy-list?pageNum=1", null, null, (res, body) => {
    console.log("add result: ",body);
});

//do get
request("GET", "http://127.0.0.1:9607/bing?q=今天天气&qs=n&form=QBRE&sp=-1&lq=0&pq=今天天气&sc=12-4&sk=&cvid=06340BD7003141AA8524FB2BF0E3F92E", null, null, (res, body) => {
    console.log(body);
});
