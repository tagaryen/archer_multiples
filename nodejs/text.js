const textHtml = 
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
  <div id="container">
  <pre>$(text_template)</pre>
  </div>
</body>
</html>`;



function handleTextHtml(title, icon, text) {
    let txt = textHtml;
    txt = txt.replace("$(title_template)", title);
    txt = txt.replace("$(icon_template)", icon);
    return txt.replace("$(text_template)", text);
}

module.exports = handleTextHtml;