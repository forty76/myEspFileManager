#pragma once 

/*
 * Auther: Muhammad Arslan
 * Dated: 15-08-2024
 * Discription: This library provides an easy way to manage the files on the flash/sd card using webpage.
 */

#include <Arduino.h>
#include <SD.h>
#include <LittleFS.h>
#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// #define EN_DEBUG
#define DEBUG_SERIAL Serial
#include <ArDebugger.h>

#define LIBRARY_VERSION "1.0.0"

class EspFileManager
{
private:
    bool memory_ready = false;
    String str_dataFS = "";
    fs::SDFS *_storage;    
    fs::LittleFSFS *_storage2;
    AsyncWebServer *_server;
    String my_path;
public:
    EspFileManager(/* args */);
    ~EspFileManager();

    // void begin(AsyncWebServer *server, FS *fs);
    void my_initLittleFS();
    //void setFileSource(SDFS *storage);
    //void setFileSource2(LittleFSFS *storage2);
    void FSlistDir(const char * dirname, uint8_t levels);
    void setServer(AsyncWebServer *server);
};

