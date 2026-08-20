const imgHtml = 
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
    #container {
      padding: 20px;
      background-color: #fff;
      border: 0px;
    }
  </style>
</head>
<body>
  <div id="container" style="display:flex; justify-content:center; align-items:center;">
  <img src="data:$(imgbase64_template)">
  </div>
</body>
</html>`;

function handleImageHtml(title, icon, contentType, imgBase64) {
    let html = imgHtml;
    html = html.replace("$(title_template)", title);
    html = html.replace("$(icon_template)", icon);
    return html.replace("$(imgbase64_template)", contentType+";base64,"+imgBase64);
}

module.exports = handleImageHtml;