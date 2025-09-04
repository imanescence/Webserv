#include "Client.hpp"

static std::string generateUniqueFileName(const std::string& originalFilename)
{
    size_t dotPos = originalFilename.find_last_of(".");
    std::string extension = "";
    std::string baseName = originalFilename;
    if (dotPos != std::string::npos)
    {
        extension = originalFilename.substr(dotPos);
        baseName = originalFilename.substr(0, dotPos);
    }
    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", localtime(&now));
    return baseName + "-" + timestamp + extension;
}

static int saveUploaderFile(const std::string& filePath, const std::string& fileContent)
{
    std::ofstream file(filePath.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "fail to open file or writing: " << filePath << std::endl;
        return WRITE_ERROR;
    }
    file.write(fileContent.c_str(), fileContent.length());
    if (!file)
    {
        std::cerr << "Error writing to file: " << filePath << std::endl;
        if (file.bad())
            std::cerr << "Stream is in bad state (badbit set)." << std::endl;
        else if (file.fail())
            std::cerr << "Logical error occurred (failbit set)." << std::endl;

        file.close();
        return WRITE_ERROR;
    }
    file.close();
    return SUCCESS;
}

static std::string extractFileName(const std::string& contentDisposition)
{
    size_t filenamePos = contentDisposition.find("filename=");
    if (filenamePos == std::string::npos)
        return "";
    size_t filenameStart = filenamePos + 9;
    if (filenameStart < contentDisposition.length() && contentDisposition[filenameStart] == '"')
        filenameStart++;
    size_t filenameEnd = contentDisposition.find("\"", filenameStart);
    if (filenameEnd == std::string::npos)
        filenameEnd = contentDisposition.find(";", filenameStart);
    if (filenameEnd == std::string::npos)
        filenameEnd = contentDisposition.length();
    if (filenameStart >= filenameEnd)
        return "";
    return contentDisposition.substr(filenameStart, filenameEnd - filenameStart);
}


static bool parseMultipartFormData(const std::string& boundary, HttpFormIn& request, std::string & uploadedFileName, std::string & uploadedFileContent)
{
    std::string startBoundary = "--" + boundary;
    std::string endBoundary = "--" + boundary + "--";
    size_t pos = 0;
    
    
    size_t nextBoundaryPos = request.getBody().find(startBoundary, pos);
    if (nextBoundaryPos == std::string::npos) {
        std::cerr << "No start boundary found" << std::endl;
        return false;
    }
    
    while (nextBoundaryPos != std::string::npos) {
        pos = nextBoundaryPos + startBoundary.length();
        while (pos < request.getBody().length() && (request.getBody()[pos] == '\r' || request.getBody()[pos] == '\n'))
            pos++;
        
        size_t nextPos = request.getBody().find(startBoundary, pos);
        if (nextPos == std::string::npos) {
            nextPos = request.getBody().find(endBoundary, pos);
            if (nextPos == std::string::npos)
                nextPos = request.getBody().length();
        }
        
        std::string part = request.getBody().substr(pos, nextPos - pos);
        size_t headersEnd = part.find("\r\n\r\n");
        if (headersEnd == std::string::npos) {
            std::cerr << "No headers end found in part" << std::endl;
            headersEnd = part.find("\n\n");
            if (headersEnd == std::string::npos)
                return false;
        }
        
        std::string headers = part.substr(0, headersEnd);
        std::string partContent;
        
        if (headersEnd + 4 < part.length())
            partContent = part.substr(headersEnd + 4);
        else if (headersEnd + 2 < part.length())
            partContent = part.substr(headersEnd + 2);
        
        while (!partContent.empty() && (partContent[partContent.length()-1] == '\r' || partContent[partContent.length()-1] == '\n'))
            partContent = partContent.substr(0, partContent.length() - 1);
        
        size_t contentDispositionPos = headers.find("Content-Disposition:");
        if (contentDispositionPos != std::string::npos) {
            std::string contentDisposition = headers.substr(contentDispositionPos);
            size_t contentDispositionEnd = contentDisposition.find("\r\n");
            if (contentDispositionEnd == std::string::npos)
                contentDispositionEnd = contentDisposition.find("\n");
            if (contentDispositionEnd != std::string::npos)
                contentDisposition = contentDisposition.substr(0, contentDispositionEnd);
            if (contentDisposition.find("filename=") != std::string::npos) {
                uploadedFileName = extractFileName(contentDisposition);
                uploadedFileContent = partContent;
            }
            else if (contentDisposition.find("name=") != std::string::npos) {
                size_t nameStart = contentDisposition.find("name=") + 5;
                if (nameStart < contentDisposition.length() && contentDisposition[nameStart] == '"')
                    nameStart++;
                size_t nameEnd = contentDisposition.find("\"", nameStart);
                if (nameEnd == std::string::npos)
                    nameEnd = contentDisposition.find(";", nameStart);
                if (nameEnd == std::string::npos)
                    nameEnd = contentDisposition.length();
                if (nameStart < nameEnd)
                    std::string fieldName = contentDisposition.substr(nameStart, nameEnd - nameStart);
            }
        }
        nextBoundaryPos = nextPos;
        if (nextPos < request.getBody().length() && request.getBody().substr(nextPos, endBoundary.length()) == endBoundary) {
            break;
        }
    }
    return true;
}

HttpFormOut Client::handlePost(HttpFormIn & request)
{
    Location location;
    std::string uploadedFileName = "";
    std::string uploadedFileContent = "";
    
    if (request.getPath().find("..") != std::string::npos){
        return HttpFormOut(FORBIDDEN, _config.getErrors());
    }

    std::string originalPath = request.getLocationPath();
    location = _config.findAndGetLocation(originalPath);
    std::map<std::string, std::string>::const_iterator contentTypeIt = request.getHeaders().find("Content-Type");
    if (contentTypeIt == request.getHeaders().end() || contentTypeIt->second.find("multipart/form-data") == std::string::npos){
        return HttpFormOut(BAD_REQUEST, _config.getErrors());
    }
    
    size_t boundaryPos = contentTypeIt->second.find("boundary=");
    if (boundaryPos == std::string::npos){
        return HttpFormOut(BAD_REQUEST, _config.getErrors());
    }
    std::string boundary = contentTypeIt->second.substr(boundaryPos + 9);
    if (!parseMultipartFormData(boundary, request, uploadedFileName, uploadedFileContent)){
        return HttpFormOut(BAD_REQUEST, _config.getErrors());
    }
    
    if (!uploadedFileName.empty() && !uploadedFileContent.empty()){
        std::string uniqueFileName = generateUniqueFileName(uploadedFileName);
        std::string uploadPath;
        if (!location.getClientBodyTmpPath().empty()) {
            uploadPath = location.getClientBodyTmpPath();
            if (!uploadPath.empty() && uploadPath[uploadPath.length() - 1] != '/') {
                uploadPath += '/';
            }
        }
        else
            uploadPath = "./";
        uploadPath += uniqueFileName;
        if (saveUploaderFile(uploadPath, uploadedFileContent) != SUCCESS)
            return HttpFormOut(INTERNAL_SERVER_ERROR, _config.getErrors());
        std::string successMessage = "<html><body><h1>Upload Success</h1><p>File " + 
                                    uploadedFileName + " has been uploaded as " + 
                                    uniqueFileName + ".</p><p><a href=\"/\">Return to home page</a></p>";
        
        std::string extension = uniqueFileName.substr(uniqueFileName.find_last_of('.') + 1);
        for (size_t i = 0; i < extension.size(); i++)
            extension[i] = std::tolower(extension[i]);
            
        if (extension == "jpg" || extension == "jpeg" || extension == "png") {
            std::string imageUrl = location.getClientBodyTmpPath();
            if (!imageUrl.empty() && imageUrl[imageUrl.length() - 1] != '/')
                imageUrl += '/';
            imageUrl += uniqueFileName;
            successMessage += "<p><img src=\"" + imageUrl + "\" alt=\"Uploaded Image\" style=\"max-width: 500px;\"></p>";
        }
        successMessage += "</body></html>";
        
        return HttpFormOut("text/html", successMessage);
    }

    return HttpFormOut(NOT_FOUND, _config.getErrors());
}
