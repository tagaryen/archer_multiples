from archernet import HttpStatusCode, HttpClient, HttpClientResponse, FormData
import json

# http://localhost:9617/archer/file-api/file-view?filename=README.md
def test_upload_file():
    form = FormData()
    form.put_file("file", "./README.md")
    
    res = HttpClient.post("http://127.0.0.1:9617/archer/file-api/file-upload?filename=README.md", 
        body=form)
    body = json.loads(str(res.content, encoding="UTF-8"))
    print(f"response: {body}")

test_upload_file()
