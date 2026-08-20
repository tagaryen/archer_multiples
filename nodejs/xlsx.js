const textHtml = 
`<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>$(title_template)</title>
    <link rel = "icon" href = "$(icon_template)" type = "image/x-icon">
    <style>
        #preview { border: 1px solid #ccc; padding: 10px; height: calc(100% - 22px); }
        .tabs {display: flex;flex-wrap: wrap;gap: 6px;margin: 0px 0px;border-bottom: 2px solid #e2e8f0;padding-bottom: 10px;}
        .tab-btn {padding: 8px 8px;border: none;background: #f1f5f9;border-radius: 8px;cursor: pointer;font-weight: 500;color: #475569;transition: 0.2s;}
        .tab-btn:hover {background: #e2e8f0;}
        .tab-btn.active {background: #3b82f6;color: white; }
        .tab-content {display: none;overflow: auto;margin-top: 10px;}
        .tab-content.active {display: block;}
        .tab-content table {border-collapse: collapse;width: 100%;font-size: 14px;}
    </style>
</head>
<body>
    <script src="/npmjs/excel"></script>
    <script src="/npmjs/xlsx"></script>
    <div id="preview"></div>
    <script type="module">
        const textBase64 = "$(xlsx_template)";
        const binaryStr = atob(textBase64);
        const bytes = new Uint8Array(binaryStr.length);
        for (let i = 0; i < binaryStr.length; i++) {bytes[i] = binaryStr.charCodeAt(i);}
        const file = new Blob([bytes], { type: 'text/plain;charset=utf-8' });
        const container = document.getElementById('preview');
        container.innerHTML = '正在加载文档...';
        try {
            const workbook = new ExcelJS.Workbook();
            await workbook.xlsx.load(file);
            const sheetNames = workbook.worksheets.map(sheet => sheet.name);
            const htmlArray = await xlsxPreview.xlsx2Html(file, {separateSheets:true});
            if (!Array.isArray(htmlArray) || sheetNames.length !== htmlArray.length) {
                console.error('❌ 渲染失败:', e);
                container.innerHTML = '文档加载失败，请检查文件格式';
            }
            let tabsHtml = '<div class="tabs">';
            let contentsHtml = '';
            sheetNames.forEach((name, index) => {
                const activeClass = index === 0 ? 'active' : '';
                tabsHtml += '<button class="tab-btn ' + activeClass + '" data-tab="'+index+'">'+name+'</button>';
                contentsHtml += '<div class="tab-content ' + activeClass + '" data-tab="'+index+'">' + (htmlArray[index] || "<p>该工作表为空</p>") + '</div>';
            });
            tabsHtml += '</div>';
            container.innerHTML = tabsHtml + contentsHtml;
            const btns = container.querySelectorAll('.tab-btn');
            const contents = container.querySelectorAll('.tab-content');
            btns.forEach(btn => {
                btn.addEventListener('click', () => {
                    const tabIndex = btn.dataset.tab;
                    btns.forEach(b => {b.classList.remove('active')});
                    btn.classList.add('active');
                    contents.forEach(c => {
                        c.classList.remove('active');
                        if(c.dataset.tab === tabIndex) c.classList.add('active');
                    });
                });
            });
            console.log('✅ 文档渲染成功');
        } catch (e) {
            console.error('❌ 渲染失败:', e);
            container.innerHTML = '文档加载失败，请检查文件格式';
        }
    </script>
</body>
</html>`;



function handleXlsxHtml(title, icon, xlsxBase64) {
    let txt = textHtml;
    txt = txt.replace("$(title_template)", title);
    txt = txt.replace("$(icon_template)", icon);
    return txt.replace("$(xlsx_template)", xlsxBase64);
}

module.exports = handleXlsxHtml;