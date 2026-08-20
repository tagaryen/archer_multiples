

const markdownHtml = 
`<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>$(title_template)</title>
    <link rel = "icon" href = "$(icon_template)" type = "image/x-icon">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 20px;
    }
    #markdown-container {
      padding: 20px;
      background-color: #fff;
      border: 0px;
    }
  </style>
  <link rel="stylesheet" href="/npmjs/hljs.css">
  <script src="/npmjs/highlight"></script>
  <script src="/npmjs/markd"></script>
</head>
<body>
  <div id="markdown-container"></div>
  <script>
    let markdownText = "$(markdown_template)";
    let binaryString = atob(markdownText);
    let bytes = new Uint8Array(binaryString.length);
    for (let i = 0; i < binaryString.length; i++) {
        bytes[i] = binaryString.charCodeAt(i);
    }
    let text = new TextDecoder('utf-8').decode(bytes);
    window.onload = () => {
      hljs.initHighlightingOnLoad();
      marked.setOptions({
          highlight: function (code) {
          return hljs.highlightAuto(code).value;
        }
      });
      document.getElementById("markdown-container").innerHTML = marked(text).replace(/<pre>/g, '<pre class="hljs">');
    }
  </script>
</body>
</html>`;

function handleMarkdownHtml(title, icon, mdTxt) {
    let base64 = Buffer.from(mdTxt, 'utf-8').toString('base64');
    let md = markdownHtml;
    md = md.replace("$(title_template)", title);
    md = md.replace("$(icon_template)", icon);
    return md.replace("$(markdown_template)", base64);
}

module.exports = handleMarkdownHtml;