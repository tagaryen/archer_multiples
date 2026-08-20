#pragma once

#include <libcommon/Common.h>
#include <libcommon/GlobalConfig.h>
#include <libserver/ServerRequest.h>
#include <libdatabase/DataBase.h>

namespace fs 
{
namespace service 
{
class FileService
{
public:

    FileService(database::DataBase & fileDB) :m_fileDB(fileDB) {}

    ~FileService() {};

    void handleFileListMessage(server::ServerRequest & request, int pageNum);

    void handleFileUploadMessage(server::ServerRequest & request, database::FileInfo & fileInfo);

    void handleFileDownoadMessage(server::ServerRequest & request, database::FileInfo & fileInfo);

    void handleFileViewMessage(server::ServerRequest & request, database::FileInfo & fileInfo);

private:
    void handleFileOuputMessage(server::ServerRequest & request, database::FileInfo & fileInfo);

    database::DataBase & m_fileDB;
};
}
}