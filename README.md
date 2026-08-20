# archer_multiples
  
## proxy manage url  
- GET /archer/proxy-api/proxy-list?pageNum=1&t=17891231243324&signature=***  
- POST /archer/proxy-api/proxy-add?t=17891231243324&signature=***  JSONBody=/build/proxy.json  
- POST /archer/proxy-api/proxy-del?name=xxxx&t=17891231243324&signature=***  
  
## file manage url  
- GET /archer/file-api/file-list?pageNum=1&t=17891231243324&signature=***  
- POST /archer/file-api/file-upload?filename=xxxx.xx&t=17891231243324&signature=***  Body=FormData  
- GET /archer/file-api/file-download?filename=xxxx.xx&t=17891231243324&signature=***  
- GET /archer/file-api/file-view?filename=xxxx.xx&t=17891231243324&signature=***  