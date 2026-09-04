function initElements() {
    document.getElementById('archerProgressBar').style.display = "none";
    document.getElementById('archerProgress').style.width = '0%';
    document.getElementById('archerProgressText').textContent = '0%';
    document.getElementById('archerFileItem').style.display = "none";
    document.getElementById('archerDropZone').style.display = "block";
    document.getElementById("archerUploadContainer").style.display = "none";
    let clearBtn = document.getElementById('archerClearBtn');
    clearBtn.disabled = false;
    clearBtn.style.opacity = "1";
    let uploadBtn = document.getElementById('archerUploadBtn');
    uploadBtn.disabled = true;
    uploadBtn.style.opacity = "0.5";
}
function createHeader(container) {
    const header = document.createElement('div');
    header.innerHTML = '<span style="font-size: 24px; line-height: 1; width: calc(100% - 30px)">📤 上传文件</span>';
    header.style.display = "flex";
    header.style.alignItems = "center";
    header.style.gap = "12px";
    header.style.marginBottom = "28px";
    const closeBtn = document.createElement('div');
    closeBtn.innerHTML = "✖";
    closeBtn.style.color = "#000";
    closeBtn.style.width = "30px";
    closeBtn.style.fontSize = "24px";
    closeBtn.style.cursor = "pointer";
    closeBtn.style.textAlign = "right";
    closeBtn.onclick = () => {
        initElements();
    };
    closeBtn.onmouseover = () => {
        closeBtn.style.color = "#f74215";
    }
    closeBtn.onmouseout = () => {
        closeBtn.style.color = "#000";
    }
    header.appendChild(closeBtn);
    container.appendChild(header);
}
function createDropZone(container, options, uploadCallback) {
    const dropZone = document.createElement('div');
    dropZone.id = "archerDropZone";
    dropZone.innerHTML = `
        <span style="font-size: 52px; display: block; margin-bottom: 16px; line-height: 1;">📁</span>
        <div style="font-size: 18px; font-weight: 500; color: #0b1a33; margin-bottom: 6px;">拖拽或选择文件</div>
        <input type="file" id="archerFileInput" multiple style="display: none;" />
        `;
    dropZone.style.border = "2px dashed #d7e0eb";
    dropZone.style.borderRadius = "20px";
    dropZone.style.padding = "52px 24px 48px";
    dropZone.style.textAlign = "center";
    dropZone.style.background = "#fafcff";
    dropZone.style.transition = "background 0.2s, border-color 0.2s";
    dropZone.style.cursor = "pointer";
    dropZone.style.position = "relative";
    function addFile(file) {
        dropZone.style.display = "none";
        let k = 1024, sizes = ['B', 'KB', 'MB', 'GB'];
        let i = Math.floor(Math.log(file.size) / Math.log(k));
        document.getElementById('archerFileName').textContent = file.name;
        document.getElementById('archerFileSize').textContent = parseFloat((file.size / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
        let fileItem = document.getElementById('archerFileItem');
        fileItem.style.display = "block";
        
        let uploadBtn = document.getElementById('archerUploadBtn');
        let clearBtn = document.getElementById('archerClearBtn');
        clearBtn.onclick = () => {
            fileItem.style.display = "none";
            uploadBtn.disabled = true;
            uploadBtn.style.opacity = "0.5";
            dropZone.style.display = "block";
        };
        uploadBtn.disabled = false;
        uploadBtn.style.opacity = "1";
        uploadBtn.onclick = () => {
            if(uploadCallback) {
                uploadCallback(file);
            }
            if(options?.showProgressBar) {
                document.getElementById('archerProgressBar').style.display = "block";
            }
            clearBtn.disabled = true;
            clearBtn.style.opacity = "0.5";
            uploadBtn.disabled = true;
            uploadBtn.style.opacity = "0.5";
        };
    }
    dropZone.addEventListener('dragover', (e) => {
        e.preventDefault();  
        e.stopPropagation();
        dropZone.style.borderColor = "#4a6cf7";
        dropZone.style.background = "#f0f4ff";
    });
    dropZone.addEventListener('dragleave', (e) => {
        e.preventDefault();  
        e.stopPropagation();
        dropZone.style.borderColor = "#d7e0eb";
        dropZone.style.background = "#fafcff";
    });
    ['dragenter', 'drop'].forEach(eventName => {  
        dropZone.addEventListener(eventName, (e) => {
            e.preventDefault();  
            e.stopPropagation();  
        });  
    }); 
    dropZone.addEventListener('drop', (e) => {
        if (e.dataTransfer.files.length === 0) return;  
        addFile(e.dataTransfer.files[0]);  
    });
    dropZone.addEventListener('mouseover', (e) => {   
        e.preventDefault();
        dropZone.style.borderColor = "#a0b8d4";
        dropZone.style.background = "#f5f9ff";
    });
    dropZone.addEventListener('mouseout', (e) => {
        e.preventDefault();
        dropZone.style.borderColor = "#d7e0eb";
        dropZone.style.background = "#fafcff";
    });
    dropZone.onclick = () => {
        const fileInput = document.getElementById('archerFileInput');
        fileInput.onchange = (e) => {
            if (e.target.files.length === 0) return;
            addFile(e.target.files[0]);
        };
        fileInput.click();
    };
    container.appendChild(dropZone);
}
function createFileItem(container) {
    const fileItem = document.createElement('div');
    fileItem.id = "archerFileItem";   
    fileItem.style.display = "none";
    fileItem.style.alignItems = "center";
    fileItem.style.justifyContent = "space-between";
    fileItem.style.padding = "12px 16px";
    fileItem.style.marginTop = "16px";
    fileItem.style.background = "#fff";
    fileItem.style.fontSize = "14px";
    fileItem.style.color = "#0b1a33";

    fileItem.innerHTML = `
        <div style="display: flex;">
        <div id="archerFileName" style="width: 65%; overflow: hidden; text-overflow: ellipsis; white-space: nowrap;"></div>
        <div id="archerFileSize" style="width: 35%; text-align: right;"></div>
        </div>
    `;

    let progressBar = document.createElement('div');
    progressBar.id = "archerProgressBar";
    progressBar.style.display = "none";
    progressBar.style.width = "100%";
    progressBar.style.height = "20px";
    progressBar.style.marginTop = "8px";
    progressBar.innerHTML = `<div style="width: 100%; height: 6px; background: #eef3fe; border-radius: 3px; border: 1px solid #ccd4f5;">
                                <div id="archerProgress" style="width: 0%; height: 100%; background: #4a6cf7; border-radius: 3px;"></div>
                            </div>
                            <div id="archerProgressText" style="text-align: center; font-size: 12px; color: #666; margin-top: 2px;">0%</div>`;
    fileItem.appendChild(progressBar);

    container.appendChild(fileItem);
}
function createActions(container) {
    const actions = document.createElement('div');
    actions.style.marginTop = "24px";
    actions.style.display = "flex";
    actions.style.justifyContent = "flex-end";
    actions.style.gap = "12px";
    actions.style.flexWrap = "wrap";
    
    let clearBtn = document.createElement('button');
    clearBtn.id = "archerClearBtn";
    clearBtn.type = "button";
    clearBtn.innerHTML = "🗑 清空";

    let uploadBtn = document.createElement('button');
    uploadBtn.id = "archerUploadBtn";
    uploadBtn.type = "button";
    uploadBtn.disabled = true;
    uploadBtn.innerHTML = "⬆ 上传";
    
    clearBtn.style.display = uploadBtn.style.display = "inline-flex";
    clearBtn.style.alignItems = uploadBtn.style.alignItems = "center";
    clearBtn.style.gap = uploadBtn.style.gap = "8px";
    clearBtn.style.padding = uploadBtn.style.padding = "10px 26px";
    clearBtn.style.borderRadius = uploadBtn.style.borderRadius = "18px";
    clearBtn.style.fontSize = uploadBtn.style.fontSize = "15px";
    clearBtn.style.fontWeight = uploadBtn.style.fontWeight = "500";
    clearBtn.style.border = uploadBtn.style.border = "none";
    clearBtn.style.color = uploadBtn.style.color = "#1f3a6b";
    clearBtn.style.transition = uploadBtn.style.transition = "background 0.15s, transform 0.1s";
    clearBtn.style.lineHeight = uploadBtn.style.lineHeight = "1.4";

    clearBtn.style.background = "#eef3fe";
    clearBtn.style.cursor = "pointer";
    uploadBtn.style.cursor = "not-allowed";
    uploadBtn.style.background = "#4a6cf7";
    uploadBtn.style.opacity = "0.5";
    uploadBtn.style.color = "#fff";

    clearBtn.addEventListener('mouseover', () => {
        clearBtn.style.background = "#e1eafe";
        clearBtn.style.borderColor = "#b0c4dd";
        if(!clearBtn.disabled) {
            clearBtn.style.cursor = "pointer";
            clearBtn.style.opacity = "0.9";
        } else {
            clearBtn.style.cursor = "not-allowed";
            clearBtn.style.opacity = "0.6";
        }
    });
    clearBtn.addEventListener('mouseout', () => {
        clearBtn.style.background = "#eef3fe";
        clearBtn.style.borderColor = "transparent";
        if(!clearBtn.disabled) {
            clearBtn.style.cursor = "pointer";
            clearBtn.style.opacity = "1";
        } else {
            clearBtn.style.cursor = "not-allowed";
            clearBtn.style.opacity = "0.5";
        }
    });
    uploadBtn.addEventListener('mouseover', () => {
        uploadBtn.style.borderColor = "#3a5de0";
        if(!uploadBtn.disabled) {
            uploadBtn.style.cursor = "pointer";
            uploadBtn.style.opacity = "0.9";
        } else {
            uploadBtn.style.cursor = "not-allowed";
            uploadBtn.style.opacity = "0.6";
        }
    });
    uploadBtn.addEventListener('mouseout', () => {
        uploadBtn.style.borderColor = "transparent";
        if(!uploadBtn.disabled) {
            uploadBtn.style.cursor = "pointer";
            uploadBtn.style.opacity = "1";
        } else {
            uploadBtn.style.cursor = "not-allowed";
            uploadBtn.style.opacity = "0.5";
        }
    });
    actions.appendChild(clearBtn);
    actions.appendChild(uploadBtn);
    container.appendChild(actions);
}

/**
 * 绘制上传窗口
 * @param {{top:number,left:number, zIndex:string, showProgressBar:boolean}} options - 上传窗口的定位和层级选项
 * @param {function(file):void} uploadCallback - 上传回调函数，接收一个参数：文件对象
*/
function drawUploadWindow(options, uploadCallback) {
    let container = document.getElementById("archerUploadContainer");
    if (container) {
        container.style.display = "block";
        return;
    }
    container = document.createElement('div');
    container.id = "archerUploadContainer";
    container.style.width = '100%';
    container.style.maxWidth = '600px';
    container.style.background = '#ffffff';
    container.style.borderRadius = '18px';
    container.style.boxShadow = '0 20px 60px rgba(0, 20, 40, 0.08), 0 8px 24px rgba(0, 0, 0, 0.04)';
    container.style.padding = '20px 36px 40px';
    container.style.transition = 'box-shadow 0.25s ease';
    container.style.zIndex = options?.zIndex || '999';
    if(options?.left !== undefined && options?.top !== undefined) {
        container.style.position = 'absolute';
        container.style.left = options.left + 'px';
        container.style.top = options.top + 'px';
    } else {
        container.style.position = 'fixed';
        container.style.left = '50%';
        container.style.top = '50%';
        container.style.transform = 'translate(-50%, -50%)';
    }
    createHeader(container);
    createDropZone(container, {showProgressBar: options?.showProgressBar}, uploadCallback);
    createFileItem(container);
    createActions(container);
    document.body.appendChild(container);
}
/**
 * 更新上传进度条
 * @param {number} percent - 上传进度百分比（0-100）
*/
function updateProgress(percent) {
    let progress = document.getElementById('archerProgress');
    let progressText = document.getElementById('archerProgressText');
    progress.style.width = percent + '%';
    progressText.textContent = percent + '%';
}
/**
 * 完成上传并关闭窗口
 * @param {{success: bool, content: string}} options - 上传完成后参数
 * @param {function():void} finishCallback - 上传完成后的回调函数
 */
function finishAndCloseUploadWindow(options, finishCallback) {
    let progress = document.getElementById('archerProgress');
    let progressText = document.getElementById('archerProgressText');
    if(!options) {
        options = {};
    }
    if(options.success) {
        progress.style.width = '100%';
        progressText.textContent = '完成';
        setTimeout(() => {
            initElements();
            if (finishCallback) {
                finishCallback();
            }
        }, 600);
    } else {
        progressText.textContent = options.content || "失败"
    }
}
export default {drawUploadWindow, updateProgress, finishAndCloseUploadWindow};