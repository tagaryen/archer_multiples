#include "FileApi.h"
#include <libservice/FileService.h>

using namespace fs::api;


void FileApi::handleRequest(server::ServerRequest & request) {
    std::string uri = request.getUri();
    std::string method = request.getMethod();

    if(method == "GET") {
        if(uri == m_listUri) {
            listFile(request);
        } else if(uri == m_downloadUri) {
            downloadFile(request);
        } else if(uri == m_viewUri) {
            viewFile(request);
        } else {
            request.sendNotFound();
        }
        return ;
    } else if(method == "POST") {
        if(uri == m_uploadUri) {
            uploadFile(request);
        } else {
            request.sendNotFound();
        }
        return ;
    }
    request.sendNotFound();
}

void FileApi::listFile(server::ServerRequest & request) {
    int pageNum = std::atoi(request.getQuery("pageNum").c_str());
    m_service.handleFileListMessage(request, pageNum);
}

void FileApi::uploadFile(server::ServerRequest & request) {
    std::string filename = request.getQuery("filename");
    if(filename.empty()) {
        request.sendOk("{\"success\":false,\"error\":\"param error\"}");
        return ;
    }
    fs::database::FileInfo fileInfo(filename, request.getAuthKey());
    if(!fileInfo.isValid()) {
        request.sendOk("{\"success\":false,\"error\":\"file system error\"}");
        return ;
    }
    m_service.handleFileUploadMessage(request, fileInfo);
}

void FileApi::downloadFile(server::ServerRequest & request) {
    std::string filename = request.getQuery("filename");
    if(filename.empty()) {
        request.sendOk("{\"success\":false,\"error\":\"param error\"}");
        return ;
    }
    fs::database::FileInfo fileInfo(filename, request.getAuthKey());
    if(!fileInfo.isValid()) {
        request.sendOk("{\"success\":false,\"error\":\"file system error\"}");
        return ;
    }
    m_service.handleFileDownoadMessage(request, fileInfo);
}

void FileApi::viewFile(server::ServerRequest & request) {
    std::string filename = request.getQuery("filename");
    if(filename.empty()) {
        request.sendOk("{\"success\":false,\"error\":\"param error\"}");
        return ;
    }
    fs::database::FileInfo fileInfo(filename, request.getAuthKey());
    if(!fileInfo.isValid()) {
        request.sendOk("{\"success\":false,\"error\":\"file system error\"}");
        return ;
    }
    m_service.handleFileViewMessage(request, fileInfo);
}