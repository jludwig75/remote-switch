#include "webserver.h"

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <GpioSwitch.h>


static GpioSwitch* _powerSwitch = NULL;

const char* switchStateToString(GpioSwitch::State state)
{
    switch (state)
    {
    case GpioSwitch::ON:
        return "ON";
    case GpioSwitch::OFF:
        return "OFF";
    default:
        return "UNKNOWN";
    }
}

ESP8266WebServer server(80);


const char* web_page = ""
"<html>\n"
"  <head>\n"
"  </head>\n"
"  <body>\n"
"    <p>\n"
"      No web page found. Please upload a web page.</p>\n"
"      <form action=\"/upload\" method=\"POST\" enctype=\"multipart/form-data\">\n"
"          <input type=\"file\"  name=\"fileToUpload\" id=\"fileToUpload\"/><br/>\n"
"          <input type=\"submit\" value=\"Upload File\" name=\"submit\"/>\n"
"      </form>\n"
"  </body>\n"
"</html>\n";

template<typename T>
class FreeBufferOnExit
{
public:
    FreeBufferOnExit(T* buffer)
        :
        _buffer(buffer)
    {
    }
    ~FreeBufferOnExit()
    {
        delete[] _buffer;
    }
private:
    T* _buffer;
};

void handleGetUpload()
{
    server.send(200, "text/html", web_page);
}

void handleRoot()
{
    File webpage = SPIFFS.open("/index.html", "r");
    if (!webpage)
    {
        handleGetUpload();
        return;
    }

    size_t webPageSize = webpage.available();
    char *pageContent = new char[webPageSize + 1];
    if (!pageContent)
    {
        server.send(500, "text/plain", "Could not allocate memory to load web page\r\n");
        return;
    }
    FreeBufferOnExit<char> onExit(pageContent);

    auto bytesRead = webpage.read(reinterpret_cast<uint8_t*>(pageContent), webPageSize);
    if (bytesRead != webPageSize)
    {
        server.send(500, "text/plain", "Error loading web page\r\n");
        return;
    }
    pageContent[webPageSize] = 0;

    server.send(200, "text/html", pageContent);
}

void handleState()
{
    if (server.method() == HTTP_GET)
    {
        server.send(200, "text/plain", switchStateToString(_powerSwitch->state()));
    }
    else if (server.method() == HTTP_POST)
    {
        if (!server.hasArg("state"))
        {
            server.send(400, "txt/plain", "\"state\" argument was not specified");
            return;
        }

        auto value = server.arg("state");
        if (value.equalsIgnoreCase("ON"))
        {
            _powerSwitch->turnOn();
        }
        else if (value.equalsIgnoreCase("OFF"))
        {
            _powerSwitch->turnOff();
        }
        else
        {
            server.send(400, "txt/plain", "Invalid value for argument \"state\": must be either \"on\" or \"off\".");
            return;
        }
        server.send(200, "text/plain", "OK");
    }
    else
    {
        server.send(405, "txt/plain", "method not supported\r\n");
    }
}

File uploadFile;

void replyServerError(String msg)
{
    Serial.println(msg);
    server.send(500, "text/plain", msg + "\r\n");
}

void handleUpload()
{
    Serial.println(String("Got upload request"));

    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        //String filename = "/index.html";
        String filename = upload.filename;
        if (!filename.startsWith("/"))
        {
            filename = "/" + filename;
        }

        Serial.println(String("handleFileUpload Name: ") + filename);
        uploadFile = SPIFFS.open(filename, "w");
        if (!uploadFile)
        {
            return replyServerError(F("CREATE FAILED"));
        }

        Serial.println(String("Upload: START, filename: ") + filename);
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadFile)
        {
            size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
            if (bytesWritten != upload.currentSize)
            {
                return replyServerError(F("WRITE FAILED"));
            }
        }
        Serial.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (uploadFile)
        {
            uploadFile.close();
        }
        Serial.println(String("Upload: END, Size: ") + upload.totalSize);
        server.sendHeader("Location", String("/"), true);
        server.send ( 302, "text/plain", "");
    }
}

void replyOK()
{
    server.send(200, "text/plain", "");
}


void webServer_setup(GpioSwitch* powerSwitch)
{
    _powerSwitch = powerSwitch;

    server.on("/", handleRoot);
    server.on("/state", handleState);
    server.on("/upload", HTTP_POST, replyOK, handleUpload);
    server.on("/upload", HTTP_GET, handleGetUpload);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
    Serial.println("HTTP server started");
}

void webServer_onLoop()
{
    server.handleClient();
}