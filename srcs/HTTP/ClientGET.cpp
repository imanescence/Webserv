#include "Client.hpp"
#include "../utlls/Utils.hpp"

static HttpFormOut extractDirList(std::string dirPath, const std::map<int, std::string> & errors) {

    if (!Utils::isValidPath(dirPath.c_str())) {
        return HttpFormOut(NOT_FOUND, errors);;
    }

    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        return HttpFormOut(NOT_FOUND, errors);
    }

    std::vector<std::string> dirList;
    struct dirent *listing;
    while ((listing = readdir(dir))) {
        
        std::string nameOfDirOrFile = listing->d_name;
        
        if (nameOfDirOrFile != "." && nameOfDirOrFile != "..")
            dirList.push_back(nameOfDirOrFile);
    }
    
    std::stringstream body;

    std::string contentType = "text/html";

    body << "<html><head><title> Dictory Listing of : " << dirPath << "</title></head><body>\n";
    body << "<h1> Directory Listing " << "</h1>\n<ul>\n";

    for (size_t i = 0; i < dirList.size(); ++i) {
        body << "<li>" << dirList[i] << "</li>\n";
    }

    body << "</ul>\n</body></html>\n";
    std::string bodyStr = body.str();

    closedir(dir);
    dirList.clear();
    return HttpFormOut(contentType, bodyStr);
}

HttpFormOut    Client::handleGet(HttpFormIn & request)
{
    std::string filePath = request.getPath();
    if (filePath.find("..") != std::string::npos){
        return HttpFormOut(FORBIDDEN, _config.getErrors());
    }
    if (Utils::isDirectory(filePath.c_str())) {
	            std::string uri = request.getUri();
	if (filePath.find_last_of('/') != filePath.size()){
            filePath.append("/");
            uri.append("/");
        }
	    if (!Utils::isValidFile((filePath + "index.html").c_str()) && _config.findAndGetLocation(uri).isAutoIndexOn()){
		    if (Utils::isValidPath(filePath.c_str()))
			return extractDirList(filePath, _config.getErrors());
	    }
	    else if (!_config.findAndGetLocation(uri).isAutoIndexOn()) {
		    if (!Utils::isValidFile((filePath + "index.html").c_str()))
			return HttpFormOut(INTERNAL_SERVER_ERROR, _config.getErrors());
		    else
            		filePath.append("index.html");
	    }
	    else {
            filePath.append("index.html");
        }
    }

    std::ifstream file(filePath.c_str(), std::ios::binary);

    if (!file){
        return HttpFormOut(NOT_FOUND, _config.getErrors());
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    request.setBody(buffer.str());
    
    std::string extension;
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filePath.substr(dotPos + 1);
        for (size_t i = 0; i < extension.size(); ++i)
            extension[i] = std::tolower(extension[i]);
    }
    std::string contentType = "application/octet-stream";
    
    if (extension == "html" || extension == "htm") {
        contentType = "text/html";
    }
    else if (extension == "css")
        contentType = "text/css";
    else if (extension == "js")
        contentType = "application/javascript";
    else if (extension == "jpg" || extension == "jpeg")
        contentType = "image/jpeg";
    else if (extension == "png")
        contentType = "image/png";
    else if (extension == "gif")
        contentType = "image/gif";
    else if (extension == "ico")
        contentType = "image/x-icon";
    else if (extension == "txt")
        contentType = "text/plain";
    HttpFormOut response(contentType, buffer.str());
    return response;
}
