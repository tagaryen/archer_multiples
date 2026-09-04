const icon = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAABhklEQVR4AexWv0vDQBR+LyBNnXQQwcHdHwj+BTrZZhIc3MXJTUGnDr2inQTBUXBwE1wEl/ijUHHo5Org" + 
"7OC/kJC0z3cXU5OY1oJJdLjjfcm7d8f7vnw5SAz446EF9B0QLXde2M65uHXaqbCdk8MHdy7rN6YESHL04QoQtplgNRUIe90udRp3bk20yeQ9mYQSAD7tENDCCB0niOiI" + 
"HLeT6tIg96J12zlutmg65AoEACx+Fh7HoDQjqiZG4WFvltdPGSoQYZmTdKcAhtcR9j3fvZeuA49QAKdB1Kr4HmRf12Zl/I0F7QLSCldvGL+NJfB6m7LJNwGyOAiiUn5i" + 
"IeuMmEOjzpFQkUb7xwQg4FRoTXRTVjkZNJnsFROgDqJPL3zAKA8AwdlQAcnFIuYxB4ogTHJoAdoB7YB2QDugHfgnDhA8J79Sec8RjFfgoRwQlnnAeaFRt0qXklAJkIn8" + 
"rwOiLUYjT6CBG4pLkjL6AjgHYZUvGCJP1NdK15IrRExAWCzynruAnx7mAwAA//9l+WMEAAAABklEQVQDAAowSFCuf/pjAAAAAElFTkSuQmCC";

const notFound = 
`<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>XY静态文件服务</title>
    <link rel = "icon" href = "data:image/png;base64,${icon}" type = "image/x-icon">
</head>
<body>
<div style="display:grid;margin-top:120px;text-align:center;">
    <span style="text-align:center;font-size:48px; margin-bottom:30px;">404 Not Found</span>
    <span style="text-align:center;font-size:18px;color: #888;margin-bottom:10px;">archer static file server</span>
    <span style="text-align:center;font-size:18px;color: #888;">$(time)</span>
</div>
</body>
</html>`;

module.exports = {
    notFound,
    icon
};