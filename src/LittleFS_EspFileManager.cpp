#include "LittleFS_EspFileManager.h"
// #include "page.h"
#include "webPage.h"
#include "FS.h"
#include <SD.h>
#include "SPI.h"
#include <LittleFS.h>

EspFileManager::EspFileManager(/* args */)
{

}

EspFileManager::~EspFileManager()
{
}

void EspFileManager::my_initLittleFS() {
    
  if (!LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  }  
    Serial.println("LittleFS mounted successfully");
}



void EspFileManager::FSlistDir(const char * dirname, uint8_t levels)
{
    DEBUGX("Listing directory: %s\n", dirname);

    File root = LittleFS.open(dirname);
    if(!root){
        DEBUGLF("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        DEBUGLF("Not a directory");
        return;
    }

    bool first_files = true;
    str_dataFS = "";
    File file = root.openNextFile();
    while(file){
        if (first_files)
            first_files = false;
        else 
            str_dataFS += ":";

        if(file.isDirectory()){
            str_dataFS += "1,";
            str_dataFS += file.name();
        } else {
            str_dataFS += "0,";
            str_dataFS += file.name();
        }
        file = root.openNextFile();
    }

    file.close();
   
}

void EspFileManager::setServer(AsyncWebServer *server)
{
    if (server == nullptr) {
        DEBUGLF("Server is null!");
        return;
    }
    _server = server;

    _server->on("/file", HTTP_GET, [&](AsyncWebServerRequest *request){ 
            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", html_page, html_page_len);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
            Serial.println(" ##################   str_dataFS in file ##########################################");
            Serial.println(str_dataFS);
                
            // request->send(200, "text/html", html_page); 
            // request->send(200, "text/plain", "Test route working");
        });

    server->on("/get-folder-contents", HTTP_GET, [&](AsyncWebServerRequest *request){
        DEBUGL2("path:", request->arg("path").c_str());
        my_path = request->arg("path").c_str();
        FSlistDir(request->arg("path").c_str(), 0);
        request->send(200, "text/plain", str_dataFS);
        Serial.println(" ################## get-folder-contents  str_dataFS in file ##########################################");
        Serial.println(str_dataFS);      
    });

    server->on("/upload", HTTP_POST, [&](AsyncWebServerRequest *request)
        { request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"File upload complete\"}"); }, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        String file_path;



        file_path = "";
        
        file_path += my_path+"/";
        
        Serial.println("¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬   "+my_path);
        file_path += filename;
        Serial.println("////////////////////////////////////   "+file_path);
        if(!index)
        {
            DEBUGX("UploadStart: %s\n", file_path.c_str());
            Serial.println(" ******************** upload ***********************************************");
            Serial.println(file_path); //%s\n
            if(LittleFS.exists(file_path)) 
            {
                LittleFS.remove(file_path);
            }
        }

        File file = LittleFS.open(file_path, FILE_APPEND);
        if(file) 
        {
            if(file.write(data, len) != len)
            {
                DEBUGLF("File write failed");
            }
            file.close();
        }
        if(final)
        {
            DEBUGX("UploadEnd: %s, %u B\n", file_path.c_str(), index+len);
        } 
    });

    server->on("/delete", HTTP_GET, [&](AsyncWebServerRequest *request){
        String path;
        if (request->hasParam("path")) 
        {
            path = request->getParam("path")->value();
        } 
        else 
        {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Path not provided\"}");
            return;
        }
        Serial.println(" ********************         Deleting File: "+ path);
        DEBUGL2("Deleting File: ", path);
        if (LittleFS.exists(path)) 
        {
            LittleFS.remove(path);
            request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"File deleted successfully\"}");
        } 
        else 
        {
            request->send(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
        } 
    });

    server->on("/download", HTTP_GET, [&](AsyncWebServerRequest *request){
        String path;
        if (request->hasParam("path")) 
        {
            path = request->getParam("path")->value();
        } 
        else 
        {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Path not provided\"}");
            return;
        }
        Serial.println(" ********************         Download File: "+ path);
        DEBUGL2("Downloading File: ", path);
        if (LittleFS.exists(path)) 
        {
            request->send(LittleFS, path, String(), true);
        } 
        else 
        {
            request->send(404, "application/json", "{\"status\":\"error\",\"message\":\"File not found\"}");
        } 
    });
}
