'use strict';

/**
 * 纯 Node.js 实现的 ZIP 解压缩工具（不依赖任何第三方库）。
 * 注：不支持 ZIP64（单文件 < 4GB、条目数 < 65535、整包 < 4GB）。
 */

const zlib = require('zlib');
const fs = require('fs');
const path = require('path');

const SIG_LOCAL = 0x04034b50; // 本地文件头
const SIG_CDIR = 0x02014b50; // 中央目录头
const SIG_EOCD = 0x06054b50; // 中央目录结束记录
const METHOD_STORE = 0;      // 不压缩
const METHOD_DEFLATE = 8;    // 原始 deflate（无 zlib 头）

const CRC32_TABLE = (() => {
  const t = new Uint32Array(256);
  for (let i = 0; i < 256; i++) {
    let c = i;
    for (let k = 0; k < 8; k++) {
      c = (c & 1) ? (0xedb88320 ^ (c >>> 1)) : (c >>> 1);
    }
    t[i] = c >>> 0;
  }
  return t;
})();

function crc32(buf) {
  let crc = 0xffffffff;
  for (let i = 0; i < buf.length; i++) {
    crc = (crc >>> 8) ^ CRC32_TABLE[(crc ^ buf[i]) & 0xff];
  }
  return (crc ^ 0xffffffff) >>> 0;
}

function toDosDateTime(date) {
  const y = date.getFullYear();
  if (y < 1980) return { time: 0, date: 0x0021 }; // 1980-01-01
  const time = (date.getHours() << 11) | (date.getMinutes() << 5) | (date.getSeconds() >>> 1);
  const dt = ((y - 1980) << 9) | ((date.getMonth() + 1) << 5) | date.getDate();
  return { time, date: dt };
}

function isAscii(s) {
  for (let i = 0; i < s.length; i++) {
    if (s.charCodeAt(i) > 127) return false;
  }
  return true;
}

/**
 * 将多个条目打包成 zip Buffer。
 * @param {Array<{name:string,data:Buffer,isDirectory?:boolean,mtime?:Date}>} entries
 * @returns {Buffer}
 */
function createZip(entries) {
  if (entries.length > 0xffff) {
    throw new Error('条目数过多（超过 65535），暂不支持 ZIP64');
  }
  const chunks = [];
  const centralDir = [];
  let offset = 0;

  for (const e of entries) {
    const nameBuf = Buffer.from(e.name, 'utf8');
    const isDir = !!e.isDirectory;
    const data = e.data || Buffer.alloc(0);
    const crc = isDir ? 0 : crc32(data);

    // 选择压缩方式：目录或空数据使用存储；否则尝试 deflate，仅在变小时才使用
    let method = METHOD_STORE;
    let compData = data;
    if (!isDir && data.length > 0) {
      const deflated = zlib.deflateRawSync(data);
      if (deflated.length < data.length) {
        method = METHOD_DEFLATE;
        compData = deflated;
      }
    }

    const { time, date } = toDosDateTime(e.mtime || new Date());
    const utf8Flag = isAscii(e.name) ? 0 : 0x0800; // bit 11：文件名为 UTF-8

    // 外部属性：Unix 风格模式（与 DOS 属性低 16 位）
    let extAttr;
    if (isDir) {
      extAttr = ((0o040755) << 16) | 0x10; // drwxr-xr-x + DOS 目录位
    } else {
      extAttr = (0o100644) << 16; // -rw-r--r--
    }

    // ---- 本地文件头 ----
    const local = Buffer.alloc(30 + nameBuf.length);
    local.writeUInt32LE(SIG_LOCAL, 0);
    local.writeUInt16LE(20, 4);            // 所需版本 2.0
    local.writeUInt16LE(utf8Flag, 6);      // 通用位标记
    local.writeUInt16LE(method, 8);
    local.writeUInt16LE(time, 10);
    local.writeUInt16LE(date, 12);
    local.writeUInt32LE(crc, 14);
    local.writeUInt32LE(compData.length, 18);
    local.writeUInt32LE(data.length, 22);
    local.writeUInt16LE(nameBuf.length, 26);
    local.writeUInt16LE(0, 28);            // 额外字段长度
    nameBuf.copy(local, 30);

    chunks.push(local, compData);

    // ---- 中央目录条目 ----
    const cd = Buffer.alloc(46 + nameBuf.length);
    cd.writeUInt32LE(SIG_CDIR, 0);
    cd.writeUInt16LE(0x0314, 4);           // 产生版本：Unix / 2.0
    cd.writeUInt16LE(20, 6);               // 所需版本
    cd.writeUInt16LE(utf8Flag, 8);
    cd.writeUInt16LE(method, 10);
    cd.writeUInt16LE(time, 12);
    cd.writeUInt16LE(date, 14);
    cd.writeUInt32LE(crc, 16);
    cd.writeUInt32LE(compData.length, 20);
    cd.writeUInt32LE(data.length, 24);
    cd.writeUInt16LE(nameBuf.length, 28);
    cd.writeUInt16LE(0, 30);               // 额外字段
    cd.writeUInt16LE(0, 32);               // 注释
    cd.writeUInt16LE(0, 34);               // 起始盘号
    cd.writeUInt16LE(0, 36);               // 内部属性
    cd.writeUInt32LE(extAttr >>> 0, 38);   // 外部属性
    cd.writeUInt32LE(offset, 42);          // 本地头偏移
    nameBuf.copy(cd, 46);

    centralDir.push(cd);
    offset += local.length + compData.length;
  }

  const cdBuf = Buffer.concat(centralDir);
  const eocd = Buffer.alloc(22);
  eocd.writeUInt32LE(SIG_EOCD, 0);
  eocd.writeUInt16LE(0, 4);               // 当前盘号
  eocd.writeUInt16LE(0, 6);               // 中央目录所在盘号
  eocd.writeUInt16LE(entries.length, 8);   // 本盘条目数
  eocd.writeUInt16LE(entries.length, 10);  // 总条目数
  eocd.writeUInt32LE(cdBuf.length, 12);    // 中央目录大小
  eocd.writeUInt32LE(offset, 16);          // 中央目录起始偏移
  eocd.writeUInt16LE(0, 20);               // 注释长度

  return Buffer.concat([...chunks, cdBuf, eocd]);
}

/**
 * 解析 zip Buffer，返回所有条目。
 * @param {Buffer} zipBuf
 * @returns {Array<{name:string,data:Buffer,isDirectory:boolean}>}
 */
function parseZip(zipBuf) {
  // 查找 EOCD（自尾部向前搜索，最多跳过 65535 字节注释）
  const minEocd = 22;
  const maxComment = 0xffff;
  let eocdOffset = -1;
  const searchStart = zipBuf.length - minEocd;
  const searchEnd = Math.max(0, zipBuf.length - minEocd - maxComment);
  for (let i = searchStart; i >= searchEnd; i--) {
    if (zipBuf[i] === 0x50 && zipBuf.readUInt32LE(i) === SIG_EOCD) {
      eocdOffset = i;
      break;
    }
  }
  if (eocdOffset < 0) throw new Error('非法 zip 数据：未找到中央目录结束标记(EOCD)');

  const total = zipBuf.readUInt16LE(eocdOffset + 10);
  const cdSize = zipBuf.readUInt32LE(eocdOffset + 12);
  let cdOffset = zipBuf.readUInt32LE(eocdOffset + 16);

  // 兼容部分工具把 cdOffset 写成相对盘的情况：若不匹配则尝试重算
  if (cdOffset + cdSize + minEocd > zipBuf.length) {
    cdOffset = eocdOffset - cdSize;
  }

  const entries = [];
  let pos = cdOffset;
  for (let i = 0; i < total; i++) {
    if (pos + 46 > zipBuf.length || zipBuf.readUInt32LE(pos) !== SIG_CDIR) {
      throw new Error('非法 zip 数据：中央目录条目损坏');
    }
    const method = zipBuf.readUInt16LE(pos + 10);
    const crc = zipBuf.readUInt32LE(pos + 16);
    const compSize = zipBuf.readUInt32LE(pos + 20);
    const uncompSize = zipBuf.readUInt32LE(pos + 24);
    const nameLen = zipBuf.readUInt16LE(pos + 28);
    const extraLen = zipBuf.readUInt16LE(pos + 30);
    const commentLen = zipBuf.readUInt16LE(pos + 32);
    const localOffset = zipBuf.readUInt32LE(pos + 42);
    const name = zipBuf.toString('utf8', pos + 46, pos + 46 + nameLen);
    pos += 46 + nameLen + extraLen + commentLen;

    // 读取本地头以定位真实数据起始
    if (localOffset + 30 > zipBuf.length || zipBuf.readUInt32LE(localOffset) !== SIG_LOCAL) {
      throw new Error('非法 zip 数据：本地文件头损坏');
    }
    const lNameLen = zipBuf.readUInt16LE(localOffset + 26);
    const lExtraLen = zipBuf.readUInt16LE(localOffset + 28);
    const dataStart = localOffset + 30 + lNameLen + lExtraLen;
    const compData = zipBuf.subarray(dataStart, dataStart + compSize);

    let data;
    if (method === METHOD_STORE) {
      data = Buffer.from(compData); // 拷贝出独立内存
    } else if (method === METHOD_DEFLATE) {
      data = zlib.inflateRawSync(compData);
    } else {
      throw new Error('不支持的压缩方式：' + method);
    }

    // 校验长度与 CRC（目录条目跳过）
    const isDir = name.endsWith('/');
    if (!isDir) {
      if (data.length !== uncompSize) {
        throw new Error('解压长度不一致：' + name);
      }
      if (crc32(data) !== crc) {
        throw new Error('CRC 校验失败：' + name);
      }
    }

    entries.push({ name, data, isDirectory: isDir });
  }
  return entries;
}

/**
 * 压缩一段内存数据 Buffer。
 * @param {Buffer} buffer 待压缩数据
 * @param {string} [entryName='data'] 条目名
 * @returns {Buffer} zip 格式 Buffer
 */
function compressBuffer(buffer, entryName = 'data') {
  if (!Buffer.isBuffer(buffer)) throw new TypeError('buffer 必须是 Buffer');
  return createZip([{ name: entryName, data: buffer, isDirectory: false, mtime: new Date() }]);
}

/**
 * 将一段 zip 格式内存 Buffer 解压，返回首个文件条目数据。
 * @param {Buffer} zipBuffer zip 格式数据
 * @returns {Buffer} 首个文件条目的原始数据
 */
function decompressBuffer(zipBuffer) {
  if (!Buffer.isBuffer(zipBuffer)) throw new TypeError('zipBuffer 必须是 Buffer');
  const entries = parseZip(zipBuffer);
  for (const e of entries) {
    if (!e.isDirectory) return e.data;
  }
  return Buffer.alloc(0);
}

/**
 * 将文件夹或文件路径压缩到给定的 zip 文件。
 * 若为目录，则保留顶层目录名作为根（与常见 zip 行为一致）。
 * @param {string} sourcePath 源文件或文件夹路径
 * @param {string} destFile  目标 zip 文件路径
 */
function compressFile(sourcePath, destFile) {
  const stat = fs.statSync(sourcePath);
  const baseName = path.basename(sourcePath);
  const entries = [];

  if (stat.isFile()) {
    entries.push({
      name: baseName,
      data: fs.readFileSync(sourcePath),
      isDirectory: false,
      mtime: stat.mtime,
    });
  } else if (stat.isDirectory()) {
    const topDir = baseName + '/';
    entries.push({ name: topDir, data: Buffer.alloc(0), isDirectory: true, mtime: stat.mtime });
    const walk = (dir, relPrefix) => {
      const items = fs.readdirSync(dir);
      for (const item of items) {
        const fullItem = path.join(dir, item);
        const st = fs.statSync(fullItem);
        const itemName = relPrefix + item; // 统一使用正斜杠
        if (st.isFile()) {
          entries.push({ name: itemName, data: fs.readFileSync(fullItem), isDirectory: false, mtime: st.mtime });
        } else if (st.isDirectory()) {
          entries.push({ name: itemName + '/', data: Buffer.alloc(0), isDirectory: true, mtime: st.mtime });
          walk(fullItem, itemName + '/');
        }
      }
    };
    walk(sourcePath, topDir);
  } else {
    throw new Error('不支持的源类型：' + sourcePath);
  }

  const zipBuf = createZip(entries);
  fs.writeFileSync(destFile, zipBuf);
}

/**
 * 将给定的 zip 文件解压到指定目录。
 * @param {string} zipFile  zip 文件路径
 * @param {string} destDir  目标目录（不存在则自动创建）
 */
function decompressFile(zipFile, destDir) {
  const zipBuf = fs.readFileSync(zipFile);
  const entries = parseZip(zipBuf);
  fs.mkdirSync(destDir, { recursive: true });
  const destResolved = path.resolve(destDir);

  for (const e of entries) {
    // 路径越界保护，防止 zip slip 攻击
    const target = path.resolve(destResolved, e.name);
    if (target !== destResolved && !target.startsWith(destResolved + path.sep)) {
      throw new Error('不安全的条目路径（越界）：' + e.name);
    }

    if (e.isDirectory) {
      fs.mkdirSync(target, { recursive: true });
    } else {
      fs.mkdirSync(path.dirname(target), { recursive: true });
      fs.writeFileSync(target, e.data);
    }
  }
}


module.exports = {
  createZip,
  parseZip,
  compressBuffer,
  decompressBuffer,
  compressFile,
  decompressFile
};
