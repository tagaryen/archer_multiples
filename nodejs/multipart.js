const http = require('http');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

/**
 * 原生 multipart/form-data 解析器（流式，文件落盘到 uploadDir）
 * 输出结构兼容 multiparty：
 *   fields: { fieldName: [value, ...] }
 *   files:  { fieldName: [{ fieldName, originalFilename, path, headers, size }, ...] }
 * @param {http.IncomingMessage} req
 * @param {string} uploadDir 临时目录
 * @param {(error: Error|null, fields: object, files: object) => void} callback
 */
function parseMultipart(req, uploadDir, callback) {
    const contentType = req.headers['content-type'] || '';
    const m = contentType.match(/boundary=(?:"([^"]+)"|([^;]+))/);
    if (!m) {
        return callback(new Error('Content-Type 缺少 boundary'), {}, {});
    }
    const boundaryStr = (m[1] || m[2]).trim();
    const boundaryBuf = Buffer.from('--' + boundaryStr);
    const sepBuf = Buffer.from('\r\n--' + boundaryStr);
    const headerSep = Buffer.from('\r\n\r\n');

    const fields = {};
    const files = {};
    let buffer = Buffer.alloc(0);
    let state = 'PREAMBLE'; // PREAMBLE | HEADERS | BODY | DONE
    let currentPart = null;
    let finished = false;
    const streams = [];

    function ensureUploadDir() {
        if (uploadDir && !fs.existsSync(uploadDir)) {
            fs.mkdirSync(uploadDir, { recursive: true });
        }
    }

    function tempFilePath() {
        ensureUploadDir();
        return path.join(uploadDir, crypto.randomBytes(16).toString('hex'));
    }

    function startPart(headerBuffer) {
        const headerStr = headerBuffer.toString('utf8');
        const headers = {};
        let name = null;
        let filename = null;
        for (const line of headerStr.split('\r\n')) {
            const idx = line.indexOf(':');
            if (idx === -1) continue;
            const key = line.slice(0, idx).trim().toLowerCase();
            const val = line.slice(idx + 1).trim();
            headers[key] = val;
            if (key === 'content-disposition') {
                const nm = val.match(/name="([^"]*)"/);
                const fm = val.match(/filename="([^"]*)"/);
                if (nm) name = nm[1];
                if (fm) filename = fm[1];
            }
        }
        currentPart = {
            headers: headers,
            name: name,
            filename: filename,
            isFile: filename !== null && filename !== undefined,
            fileSize: 0,
            fileStream: null,
            fieldBuffer: Buffer.alloc(0),
            tmpPath: null
        };
    }

    function writeBody(data) {
        if (!currentPart) return;
        if (currentPart.isFile) {
            if (!currentPart.fileStream) {
                currentPart.tmpPath = tempFilePath();
                currentPart.fileStream = fs.createWriteStream(currentPart.tmpPath);
                streams.push(currentPart.fileStream);
            }
            currentPart.fileStream.write(data);
            currentPart.fileSize += data.length;
        } else {
            currentPart.fieldBuffer = Buffer.concat([currentPart.fieldBuffer, data]);
        }
    }

    function finalizePart() {
        if (!currentPart) return;
        if (currentPart.isFile) {
            if (currentPart.fileStream) currentPart.fileStream.end();
            if (!files[currentPart.name]) files[currentPart.name] = [];
            files[currentPart.name].push({
                fieldName: currentPart.name,
                originalFilename: currentPart.filename,
                path: currentPart.tmpPath,
                headers: currentPart.headers,
                size: currentPart.fileSize
            });
        } else {
            if (!fields[currentPart.name]) fields[currentPart.name] = [];
            fields[currentPart.name].push(currentPart.fieldBuffer.toString('utf8'));
        }
        currentPart = null;
    }

    function finish(err) {
        if (finished) return;
        finished = true;
        if (streams.length === 0) {
            return callback(err, fields, files);
        }
        let pending = streams.length;
        const done = () => {
            if (pending === 0) return;
            pending--;
            if (pending === 0) callback(err, fields, files);
        };
        for (const s of streams) {
            if (s.closed) {
                pending--;
            } else {
                s.once('close', done);
            }
        }
        if (pending === 0) callback(err, fields, files);
    }

    function process() {
        while (state !== 'DONE') {
            if (state === 'PREAMBLE') {
                const idx = buffer.indexOf(boundaryBuf);
                if (idx === -1) {
                    // 保留尾部，避免 boundary 被切断
                    const keep = Math.min(buffer.length, boundaryBuf.length);
                    buffer = buffer.slice(buffer.length - keep);
                    return;
                }
                let after = idx + boundaryBuf.length;
                // 结束边界 --boundary--
                if (buffer.slice(after, after + 2).toString() === '--') {
                    state = 'DONE';
                    finish(null);
                    return;
                }
                // 跳过 boundary 后的 CRLF
                if (buffer.slice(after, after + 2).toString() === '\r\n') after += 2;
                buffer = buffer.slice(after);
                state = 'HEADERS';
            } else if (state === 'HEADERS') {
                const idx = buffer.indexOf(headerSep);
                if (idx === -1) return; // 等待更多数据
                startPart(buffer.slice(0, idx));
                buffer = buffer.slice(idx + headerSep.length);
                state = 'BODY';
            } else if (state === 'BODY') {
                const idx = buffer.indexOf(sepBuf);
                if (idx === -1) {
                    // 保留尾部，避免 \r\n--boundary 被切断
                    const keep = Math.min(buffer.length, sepBuf.length);
                    if (buffer.length > keep) {
                        writeBody(buffer.slice(0, buffer.length - keep));
                        buffer = buffer.slice(buffer.length - keep);
                    }
                    return;
                }
                writeBody(buffer.slice(0, idx));
                let after = idx + sepBuf.length;
                // 结束边界
                if (buffer.slice(after, after + 2).toString() === '--') {
                    finalizePart();
                    state = 'DONE';
                    finish(null);
                    return;
                }
                // 跳过 CRLF
                if (buffer.slice(after, after + 2).toString() === '\r\n') after += 2;
                buffer = buffer.slice(after);
                finalizePart();
                state = 'HEADERS';
            }
        }
    }

    req.on('data', (chunk) => {
        buffer = Buffer.concat([buffer, chunk]);
        process();
    });
    req.on('end', () => {
        process();
        if (!finished) finish(null);
    });
    req.on('error', (err) => {
        finish(err);
    });
}

module.exports = {
    parseMultipart
}