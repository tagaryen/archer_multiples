#include "ProxyHandler.h"
#include <libservice/FileService.h>

using namespace fs::handler;

static std::map<std::string, std::string> mimeTypes = {
    {".ai","application/postscript"},    {".js","application/ecmascript"},    {".edi","application/EDI-X12"},    {".json","application/json"},    {".ogg","application/ogg"},
    {".woff","application/font-woff"},    {".xhtml","application/xhtml+xml"},    {".xml","application/xml"},    {".zip","application/zip"},    {".gzip","application/gzip"},
    {".001","application/x-001"},    {".301","application/x-301"},    {".906","application/x-906"},    {".a11","application/x-a11"},    {".awf","application/vnd.adobe.workflow"},
    {".bmp","application/x-bmp"},    {".c4t","application/x-c4t"},    {".cal","application/x-cals"},    {".cdf","application/x-netcdf"},    {".cel","application/x-cel"},
    {".cg4","application/x-g4"},    {".cit","application/x-cit"},    {".bot","application/x-bot"},    {".c90","application/x-c90"},    {".cat","application/vnd.ms-pki.seccat"},
    {".cdr","application/x-cdr"},    {".cer","application/x-x509-ca-cert"},    {".cgm","application/x-cgm"},    {".cmx","application/x-cmx"},    {".crl","application/pkix-crl"},
    {".csi","application/x-csi"},    {".cut","application/x-cut"},    {".dbm","application/x-dbm"},    {".cmp","application/x-cmp"},    {".cot","application/x-cot"},
    {".crt","application/x-x509-ca-cert"},    {".dbf","application/x-dbf"},    {".dbx","application/x-dbx"},    {".dcx","application/x-dcx"},    {".dgn","application/x-dgn"},
    {".dll","application/x-msdownload"},    {".dot","application/msword"},    {".der","application/x-x509-ca-cert"},    {".dib","application/x-dib"},    {".doc","application/msword"},
    {".drw","application/x-drw"},    {".dwf","application/x-dwf"},    {".dxb","application/x-dxb"},    {".edn","application/vnd.adobe.edn"},    {".dwg","application/x-dwg"},
    {".dxf","application/x-dxf"},    {".emf","application/x-emf"},    {".epi","application/x-epi"},    {".exe","application/x-msdownload"},    {".fdf","application/vnd.fdf"},
    {".eps","application/x-ps"},    {".etd","application/x-ebx"},    {".fif","application/fractals"},    {".frm","application/x-frm"},    {".gbr","application/x-gbr"},
    {".g4","application/x-g4"},    {".gl2","application/x-gl2"},    {".hgl","application/x-hgl"},    {".hpg","application/x-hpgl"},    {".hqx","application/mac-binhex40"},
    {".hta","application/hta"},    {".gp4","application/x-gp4"},    {".hmr","application/x-hmr"},    {".hpl","application/x-hpl"},    {".hrf","application/x-hrf"},
    {".icb","application/x-icb"},    {".ig4","application/x-g4"},    {".iii","application/x-iphone"},    {".ins","application/x-internet-signup"},    {".iff","application/x-iff"},
    {".igs","application/x-igs"},    {".img","application/x-img"},    {".isp","application/x-internet-signup"},    {".lar","application/x-laplayer-reg"},    {".latex","application/x-latex"},
    {".lbm","application/x-lbm"},    {".ls","application/x-javascript"},    {".ltr","application/x-ltr"},    {".man","application/x-troff-man"},    {".mdb","application/msaccess"},
    {".mac","application/x-mac"},    {".mfp","application/x-shockwave-flash"},    {".mi","application/x-mi"},    {".mil","application/x-mil"},    {".mocha","application/x-javascript"},
    {".mpd","application/vnd.ms-project"},    {".mpp","application/vnd.ms-project"},    {".mpt","application/vnd.ms-project"},    {".mpw","application/vnd.ms-project"},    {".mpx","application/vnd.ms-project"},
    {".mxp","application/x-mmxp"},    {".nrf","application/x-nrf"},    {".out","application/x-out"},    {".p12","application/x-pkcs12"},    {".p7c","application/pkcs7-mime"},
    {".p7r","application/x-pkcs7-certreqresp"},    {".pc5","application/x-pc5"},    {".pcl","application/x-pcl"},    {".pdx","application/vnd.adobe.pdx"},    {".pgl","application/x-pgl"},
    {".pko","application/vnd.ms-pki.pko"},    {".p10","application/pkcs10"},    {".p7b","application/x-pkcs7-certificates"},    {".p7m","application/pkcs7-mime"},    {".p7s","application/pkcs7-signature"},
    {".pci","application/x-pci"},    {".pcx","application/x-pcx"},    {".pdf","application/pdf"},    {".pfx","application/x-pkcs12"},    {".pic","application/x-pic"},
    {".pl","application/x-perl"},    {".plt","application/x-plt"},    {".ppa","application/vnd.ms-powerpoint"},    {".pps","application/vnd.ms-powerpoint"},    {".prf","application/pics-rules"},
    {".prt","application/x-prt"},    {".pwz","application/vnd.ms-powerpoint"},    {".ras","application/x-ras"},    {".pot","application/vnd.ms-powerpoint"},    {".ppm","application/x-ppm"},
    {".ppt","application/vnd.ms-powerpoint"},    {".pr","application/x-pr"},    {".prn","application/x-prn"},    {".ps","application/x-ps"},    {".ptn","application/x-ptn"},
    {".red","application/x-red"},    {".rjs","application/vnd.rn-realsystem-rjs"},    {".rlc","application/x-rlc"},    {".rm","application/vnd.rn-realmedia"},    {".rat","application/rat-file"},
    {".rec","application/vnd.rn-recording"},    {".rgb","application/x-rgb"},    {".rjt","application/vnd.rn-realsystem-rjt"},    {".rle","application/x-rle"},    {".rmf","application/vnd.adobe.rmf"},
    {".rmj","application/vnd.rn-realsystem-rmj"},    {".rmp","application/vnd.rn-rn_music_package"},    {".rmvb","application/vnd.rn-realmedia-vbr"},    {".rnx","application/vnd.rn-realplayer"},    {".rms","application/vnd.rn-realmedia-secure"},
    {".rmx","application/vnd.rn-realsystem-rmx"},    {".rsml","application/vnd.rn-rsml"},    {".rtf","application/msword"},    {".sat","application/x-sat"},    {".sdw","application/x-sdw"},
    {".slb","application/x-slb"},    {".sam","application/x-sam"},    {".sdp","application/sdp"},    {".sit","application/x-stuffit"},    {".sld","application/x-sld"},
    {".smi","application/smil"},    {".smk","application/x-smk"},    {".smil","application/smil"},    {".spc","application/x-pkcs7-certificates"},    {".spl","application/futuresplash"},
    {".ssm","application/streamingmedia"},    {".stl","application/vnd.ms-pki.stl"},    {".sst","application/vnd.ms-pki.certstore"},    {".tdf","application/x-tdf"},    {".tga","application/x-tga"},
    {".sty","application/x-sty"},    {".swf","application/x-shockwave-flash"},    {".tg4","application/x-tg4"},    {".vdx","application/vnd.visio"},    {".vpg","application/x-vpeg005"},
    {".vst","application/vnd.visio"},    {".vsw","application/vnd.visio"},    {".vtx","application/vnd.visio"},    {".torrent","application/x-bittorrent"},    {".vda","application/x-vda"},
    {".vsd","application/vnd.visio"},    {".vss","application/vnd.visio"},    {".vsx","application/vnd.visio"},    {".wb1","application/x-wb1"},    {".wb3","application/x-wb3"},
    {".wiz","application/msword"},    {".wk4","application/x-wk4"},    {".wks","application/x-wks"},    {".wb2","application/x-wb2"},    {".wk3","application/x-wk3"},
    {".wkq","application/x-wkq"},    {".wmf","application/x-wmf"},    {".wmd","application/x-ms-wmd"},    {".wp6","application/x-wp6"},    {".wpg","application/x-wpg"},
    {".wq1","application/x-wq1"},    {".wri","application/x-wri"},    {".ws","application/x-ws"},    {".wmz","application/x-ms-wmz"},    {".wpd","application/x-wpd"},
    {".wpl","application/vnd.ms-wpl"},    {".wr1","application/x-wr1"},    {".wrk","application/x-wrk"},    {".ws2","application/x-ws"},    {".xdp","application/vnd.adobe.xdp"},
    {".xfd","application/vnd.adobe.xfd"},    {".xfdf","application/vnd.adobe.xfdf"},    {".xls","application/vnd.ms-excel"},    {".xwd","application/x-xwd"},    {".sis","application/vnd.symbian.install"},
    {".x_t","application/x-x_t"},    {".apk","application/vnd.android.package-archive"},    {".x_b","application/x-x_b"},    {".sisx","application/vnd.symbian.install"},    {".ipa","application/vnd.iphone"},
    {".xap","application/x-silverlight-app"},    {".xlw","application/x-xlw"},    {".anv","application/x-anv"},    {".uin","application/x-icq"},    {".323","text/h323"},
    {".biz","text/xml"},    {".cml","text/xml"},    {".asa","text/asa"},    {".asp","text/asp"},    {".css","text/css"},
    {".csv","text/csv"},    {".dcd","text/dcd"},    {".dtd","text/dtd"},    {".ent","text/ent"},    {".fo","text/fo"},
    {".htc","text/x-component"},    {".html","text/html"},    {".htx","text/htx"},    {".htm","text/htm"},    {".htt","text/webviewhtml"},
    {".jsp","text/jsp"},    {".math","text/xml"},    {".md","text/markdown"},    {".mml","text/mml"},    {".mtx","text/xml"},
    {".plg","text/plg"},    {".rdf","text/xml"},    {".rt","text/vnd.rn-realtext"},    {".sol","text/plain"},    {".spp","text/xml"},
    {".stm","text/html"},    {".tld","text/xml"},    {".txt","text/plain"},    {".uls","text/iuls"},    {".vml","text/xml"},
    {".tsd","text/xml"},    {".vcf","text/x-vcard"},    {".vxml","text/xml"},    {".wml","text/vnd.wap.wml"},    {".wsdl","text/xml"},
    {".wsc","text/scriptlet"},    {".xdr","text/xdr"},    {".xql","text/xql"},    {".xsd","text/xsd"},    {".xslt","text/xslt"},
    {".xq","text/xq"},    {".xquery","text/xquery"},    {".xsl","text/xsl"},    {".odc","text/x-ms-odc"},    {".r3t","text/vnd.rn-realtext3d"},
    {".sor","text/plain"},    {".tif","image/tiff"},    {".fax","image/fax"},    {".gif","image/gif"},    {".ico","image/x-icon"},
    {".jfif","image/jpeg"},    {".jpe","image/jpeg"},    {".jpeg","image/jpeg"},    {".jpg","image/jpeg"},    {".net","image/pnetvue"},
    {".png","image/png"},    {".rp","image/vnd.rn-realpix"},    {".svg","image/svg+xml"},    {".tiff","image/tiff"},    {".wbmp","image/vnd.wap.wbmp"}
};


void proxyRequestSender(RequestSender *sender, void *arg) {
    fs::server::ServerRequest *request = static_cast<fs::server::ServerRequest *>(arg);
    char buf[8 * 1024 + 1];
    int reads = 0;
    while((reads = request->readBytes(buf, 1024 * 8)) > 0) {
        buf[reads] = '\0';
        http_request_sender_send(sender, buf, reads);
    }
    http_request_sender_end(sender);
}
void proxyResponseReader(Response *res, const void *data, const size_t size, void *arg) {
    fs::server::ServerRequest *request = static_cast<fs::server::ServerRequest *>(arg);
    
    if(http_client_response_has_err(res)) {
        LOG_error("Proxy request error, %s", http_client_response_errorstr(res));
        request->sendServiceUnavailable();
        return ;
    }

    if(!request->headerSended()) {
        unsigned int size = http_client_response_header_size(res);
        for(int i = 0; i < size; i++) {
            const char *key = http_client_response_get_header_key(res,i);
            if(strcasecmp("transfer-encoding", key) == 0 || strcasecmp("content-length", key) == 0) {
                continue;
            }
            request->setResponseHeader(key, http_client_response_get_header_val(res,i));
        }
        request->beginResponse(http_client_response_get_status(res));
    }
    if(size == 0) {
        request->endResponse();
    } else {
        request->sendBytes((const char *)data, size);
    }
}

ProxyHandler::ProxyHandler(std::string const & body) {
    m_valid = false;

    Json::Value proxyJson;
    Json::Reader reader;
    if(!reader.parse(body.c_str(), body.c_str() + body.length(), proxyJson)) {
        LOG_warn("Proxy parse body failed, body: %s", body.c_str());
        return ;
    }
    m_raw = body;
    if(proxyJson.isMember("name")) {
        setId(proxyJson["name"].asString());
    } else {
        LOG_warn("Proxy name is null");
        return ;
    }
    if(proxyJson.isMember("requestPath")) {
        m_requestPath = proxyJson["requestPath"].asString();
    } else {
        LOG_warn("Proxy requestPath is null");
        return ;
    }
    bool hasLocation = false;
    if(proxyJson.isMember("location")) {
        m_location = proxyJson["location"].asString();
        hasLocation = true;
        if(!common::isAbsolutePath(m_location)) {
            m_location = common::getCurrentPath() + m_location;
        }
        if(!common::createDirectories2(m_location)) {
            LOG_warn("Proxy can not create dir %s", m_location.c_str());
            return ;
        }
        if(m_location[m_location.length()-1] == '/') {
            m_location = m_location.substr(0, m_location.length()-1);
        }
    }
    if(!hasLocation && proxyJson.isMember("proxyUrl")) {
        m_proxyUrl = proxyJson["proxyUrl"].asString();
        if(!parseUrl()) {
            LOG_warn("Proxy proxyUrl invalid %s", m_proxyUrl.c_str());
            return;
        }
    } else {
        LOG_warn("Proxy either proxyUrl or location is required");
        return ;
    }
    if(m_ssl && proxyJson.isMember("proxySsl")) {
        Json::Value ssl = proxyJson["proxySsl"];
        std::string caPath, crtPath, keyPath, enCrtPath, enKeyPath;
        bool verifyPeer = true;
        if(ssl.isMember("verifyPeer")) {
            verifyPeer = ssl["verifyPeer"].asBool();
        }
        if(ssl.isMember("caPath")) {
            caPath = ssl["caPath"].asString();
        }
        if(ssl.isMember("crtPath")) {
            crtPath = ssl["crtPath"].asString();
        }
        if(ssl.isMember("keyPath")) {
            keyPath = ssl["keyPath"].asString();
        }
        if(ssl.isMember("enCrtPath")) {
            enCrtPath = ssl["enCrtPath"].asString();
        }
        if(ssl.isMember("enKeyPath")) {
            enKeyPath = ssl["enKeyPath"].asString();
        }

        m_ssloption = ssl_option_new(1, verifyPeer);

        if(caPath.empty()) {
            LOG_warn("ProxySsl.caPath can not be found");
        } else {
            std::ifstream ca(caPath);
            if(!ca.is_open()) {
                LOG_error("Can not open file %s", crtPath.c_str());
            } else {
                std::stringstream caBuffer; 
                caBuffer << ca.rdbuf();
                std::string caStr =  caBuffer.str();
                if(ssl_option_set_trust_ca(m_ssloption, caStr.c_str(), caStr.length()) == 0) {
                    LOG_error("Can not set SSL ca file %s", caStr.c_str());
                }
            }
        }

        if(crtPath.empty()) {
            LOG_warn("ProxySsl.crtPath can not be found");
        } else if(keyPath.empty()) {
            LOG_warn("ProxySsl.keyPath can not be found");
        } else if(m_ssloption) {
            bool ok = true;
            std::ifstream crt(crtPath), key(keyPath);
            if(!crt.is_open()) {
                LOG_error("Can not open file %s", crtPath.c_str());
                ok = false;
            }
            if(!key.is_open()) {
                LOG_error("Can not open file %s", keyPath.c_str());
                ok = false;
            }
            if(ok) {
                std::stringstream crtBuffer, keyBuffer; 
                crtBuffer << crt.rdbuf();
                keyBuffer << key.rdbuf();
                std::string crtStr =  crtBuffer.str(), keyStr = keyBuffer.str();
                if(ssl_option_set_certificate_and_key(m_ssloption, crtStr.c_str(), crtStr.length(), keyStr.c_str(), keyStr.length()) == 0) {
                    LOG_error("Can not set SSL Crt and Key file");
                }
            }
            crt.close();
            key.close();
        }
    
        if(enCrtPath.empty()) {
            LOG_warn("ProxySsl.enCrtPath can not be found");
        } else if(enKeyPath.empty()) {
            LOG_warn("ProxySsl.enKeyPath can not be found");
        } else if(m_ssloption) {
            bool ok = true;
            std::ifstream enCrt(enCrtPath), enKey(enKeyPath);
            if(!enCrt.is_open()) {
                LOG_error("Can not open file %s", enCrtPath.c_str());
                ok = false;
            }
            if(!enKey.is_open()) {
                LOG_error("Can not open file %s", enKeyPath.c_str());
                ok = false;
            }
            if(ok) {
                std::stringstream crtBuffer, keyBuffer; 
                crtBuffer << enCrt.rdbuf();
                keyBuffer << enKey.rdbuf();
                std::string crtStr =  crtBuffer.str(), keyStr = keyBuffer.str();
                if(ssl_option_set_encrypt_certificate_and_key(m_ssloption, crtStr.c_str(), crtStr.length(), keyStr.c_str(), keyStr.length()) == 0) {
                    LOG_error("Can not set Encrypt SSL Crt and Key file");
                }
            }
            enCrt.close();
            enKey.close();
        }
    }
    if(proxyJson.isMember("requestHeaders")) {
        Json::Value requestHeadersJson = proxyJson["requestHeaders"];
        if(!requestHeadersJson.isObject()) {
            LOG_warn("Proxy requestHeaders must be an object");
            return ;
        }
        for (const auto& key : requestHeadersJson.getMemberNames()) {  
            m_requestHeaders.emplace_back(std::pair<std::string, std::string>(key, requestHeadersJson[key].asString()));
        } 
    }
    if(proxyJson.isMember("responseHeaders")) {
        Json::Value responseHeadersJson = proxyJson["responseHeaders"];
        if(!responseHeadersJson.isObject()) {
            LOG_warn("Proxy responseHeaders must be an object");
            return ;
        }
        for (const auto& key : responseHeadersJson.getMemberNames()) {  
            m_responseHeaders.emplace_back(std::pair<std::string, std::string>(key, responseHeadersJson[key].asString()));
        } 
    }
    m_valid = true;
}

bool ProxyHandler::parseUrl() {
    if(m_proxyUrl.length() < 10) {
        return false;
    }
    std::string host = "";
    if(m_proxyUrl.substr(0, 8) == "https://") {
        m_ssl = true;
        host = m_proxyUrl.substr(8);
    } else if(m_proxyUrl.substr(0, 7) == "http://") {
        host = m_proxyUrl.substr(7);
    } else {
        return false;
    }
    size_t idx = host.find_first_of('/');
    if(idx != std::string::npos) {
        host = host.substr(0, idx);
    }
    idx = host.find_first_of(':');
    if(idx == std::string::npos) {
        host += m_ssl?":443":":80";
    }
    m_host = host;
    return true;
}

void ProxyHandler::handleRequest(server::ServerRequest & request) {
    if(m_location.empty()) {
        handleProxy(request);
    } else {
        handleLocation(request);
    }
}


void ProxyHandler::handleLocation(server::ServerRequest & request) {
    std::string uri = request.getUri();
    std::string method = request.getMethod();
    if(method != "GET") {
        LOG_warn("Proxy location method is NOT GET, response 'not found' directly");
        request.sendNotFound();
        return ;
    }
    if(!request.hasReferer()) {
        size_t pos = 0;
        if((pos = uri.find(m_requestPath)) != std::string::npos) {
            uri.erase(pos, m_requestPath.length());
        }
    }
    std::string filePath = m_location + uri;

    if(common::isDirectory(filePath)) {
        if(filePath[filePath.length() - 1] != '/') {
            filePath += "/";
        }
        filePath += "index.html";
    }

    LOG_trace("Proxy location requestPath = %s, filePath = %s", m_requestPath.c_str(), filePath.c_str());

    if(common::fileExists(filePath) == false) {
        LOG_warn("Proxy location %s not found", filePath.c_str());
        request.sendNotFound();
        return;
    } 
    
    FILE *file = fopen(filePath.c_str(), "rb");
    if(!file) {
        LOG_error("Can not open file %s", filePath.c_str());
        request.sendInternalError();
        return ;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);  

    int fd = fileno(file);
    
    std::string type = "application/none";
    size_t dotPos = filePath.find_last_of('.');
    if(dotPos != std::string::npos) {
        type = mimeTypes[filePath.substr(dotPos).c_str()];
    }
    if(type.empty()) {
        type = "application/none";
    }
    request.setResponseHeader("content-type", type.c_str());

    request.setResponseHeader("Content-Length", std::to_string(fileSize).c_str());
    request.sendFile(fd, fileSize);

    fclose(file);
}

void ProxyHandler::handleProxy(server::ServerRequest & request) {
    std::string url = m_proxyUrl + request.getUriWithQuery().substr(m_requestPath.length());
    std::string method = request.getMethod();
    
    HttpClientOption *opt = http_client_opt_new_method(method.c_str());

    for(unsigned int i = 0; i < request.getHeaderSize(); i++) {
        std::string k = request.getHeaderKey(i);
        if(!k.empty()) {
            if(strcasecmp(k.c_str(), "content-length") == 0) {
                continue;
            }
            http_client_opt_add_header(opt, k.c_str(), request.getHeaderVal(i).c_str());
        }
    }

    for(auto const& p: m_requestHeaders) {
        http_client_opt_add_header(opt, p.first.c_str(), p.second.c_str());
    }

    http_client_opt_add_header(opt, "host", m_host.c_str());

    for(auto const& p: m_responseHeaders) {
        request.setResponseHeader(p.first.c_str(), p.second.c_str());
    }
    http_client_opt_add_cb_arg(opt, &request);

    LOG_trace("Proxy request host = %s method = %s url = %s", m_host.c_str(), method.c_str(), url.c_str());
    if(m_ssloption) {
        http_client_opt_set_ssloption(opt, m_ssloption);
    }

    http_client_stream_request(url.c_str(), opt, proxyRequestSender, proxyResponseReader);
    http_client_opt_free(opt);
}