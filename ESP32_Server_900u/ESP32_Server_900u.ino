#include <FS.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <DNSServer.h>
#include <Update.h>
#include "Loader.h"
#include "Pages.h"
#include "exfathax.h"
#include "USB.h"
#include "USBMSC.h"

/*
#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC USBSerial;
#endif
*/

                    // enable internal goldhen.h [ true / false ]
#define INTHEN true // goldhen is placed in the app partition to free up space on spiffs for other payloads, target partition scheme: [No OTA (1MB APP/3MB SPIFFS)]
                    // with this enabled you do not upload goldhen to the board, set this to false if you wish to upload goldhen

#if INTHEN
#include "goldhen.h"
#endif

String AP_SSID = "PS4_WEB_AP";
String AP_PASS = "password";
int WEB_PORT = 80;
IPAddress Server_IP(10,1,1,1);
IPAddress Subnet_Mask(255,255,255,0);

String firmwareVer = "1.00";
DNSServer dnsServer;
AsyncWebServer server(WEB_PORT);
boolean hasEnabled = false;
long enTime = 0;
File upFile;
USBMSC dev;


String split(String str, String from, String to)
{
  String tmpstr = str;
  tmpstr.toLowerCase();
  from.toLowerCase();
  to.toLowerCase();
  int pos1 = tmpstr.indexOf(from);
  int pos2 = tmpstr.indexOf(to, pos1 + from.length());   
  String retval = str.substring(pos1 + from.length() , pos2);
  return retval;
}


bool instr(String str, String search)
{
int result = str.indexOf(search);
if (result == -1)
{
  return false;
}
return true;
}


String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+" B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+" KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+" MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+" GB";
  }
}


String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
      }
      yield();
    }
    encodedString.replace("%2E",".");
    return encodedString;
}


String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  else if(filename.endsWith(".bin")) return "application/octet-stream";
  else if(filename.endsWith(".manifest")) return "text/cache-manifest";
  return "text/plain";
}


void sendwebmsg(AsyncWebServerRequest *request, String htmMsg)
{
    String tmphtm = "<!DOCTYPE html><html><head><link rel=\"stylesheet\" href=\"style.css\"></head><center><br><br><br><br><br><br>" + htmMsg + "</center></html>";
    request->send(200, "text/html", tmphtm);
}


void handleFwUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if(!index){
      String path = request->url();
      if (path != "/update.html") {
        request->send(500, "text/plain", "Internal Server Error");
        return;
      }
      if (!filename.equals("fwupdate32.bin")) {
        sendwebmsg(request, "Invalid update file: " + filename);
        return;
      }
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      //HWSerial.printf("Update Start: %s\n", filename.c_str());
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        Update.printError(Serial);
        sendwebmsg(request, "Update Failed: " + String(Update.errorString()));
      }
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        Update.printError(Serial);
        sendwebmsg(request,  "Update Failed: " + String(Update.errorString()));
      }
    }
    if(final){
      if(Update.end(true)){
        //HWSerial.printf("Update Success: %uB\n", index+len);
        String tmphtm = "<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"8; url=/info.html\"><link rel=\"stylesheet\" href=\"style.css\"></head><center><br><br><br><br><br><br>Update Success, Rebooting.</center></html>";
        request->send(200, "text/html", tmphtm);
        delay(1000);
        ESP.restart();
      } else {
        Update.printError(Serial);
      }
    }
}


void handleDelete(AsyncWebServerRequest *request){
  if(!request->hasParam("file", true))
  {
    request->redirect("/fileman.html"); 
    return;
  }
  String path = request->getParam("file", true)->value();
  if(path.length() == 0) 
  {
    request->redirect("/fileman.html"); 
    return;
  }
  if (SPIFFS.exists("/" + path) && path != "/" && !path.equals("config.ini")) {
    SPIFFS.remove("/" + path);
  }
  request->redirect("/fileman.html"); 
}



void handleFileMan(AsyncWebServerRequest *request) {
  File dir = SPIFFS.open("/");
  String output = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>File Manager</title><link rel=\"stylesheet\" href=\"style.css\"><script>function statusDel(fname) {var answer = confirm(\"Are you sure you want to delete \" + fname + \" ?\");if (answer) {return true;} else { return false; }}</script></head><body><br><table>"; 
  int fileCount = 0;
  File file = dir.openNextFile();
  while(file){
    String fname = String(file.name());
    if (fname.length() > 0 && !fname.equals("config.ini"))
    {
    fileCount++;
    output += "<tr>";
    output += "<td><a href=\"" +  fname + "\">" + fname + "</a></td>";
    output += "<td>" + formatBytes(file.size()) + "</td>";
    output += "<td><form action=\"/" + fname + "\" method=\"get\"><button type=\"submit\">Download</button></form></td>";
    output += "<td><form action=\"/delete\" method=\"post\"><button type=\"submit\" name=\"file\" value=\"" + fname + "\" onClick=\"return statusDel('" + fname + "');\">Delete</button></form></td>";
    output += "</tr>";
    }
    file.close();
    file = dir.openNextFile();
  }
  if (fileCount == 0)
  {
      output += "<p><center>No files found<br>You can upload files using the \"File Uploader\" page.</center></p>";
  }
  output += "</table></body></html>";
  request->send(200, "text/html", output);
}


void handlePayloads(AsyncWebServerRequest *request) {
  File dir = SPIFFS.open("/");
  String output = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>ESP Server</title><link rel=\"stylesheet\" href=\"style.css\"><style>body { background-color: #1451AE; color: #ffffff; font-size: 14px; font-weight: bold; margin: 0 0 0 0.0; overflow-y:hidden; text-shadow: 3px 2px DodgerBlue;}</style><script>function setpayload(payload,title,waittime){ sessionStorage.setItem('payload', payload); sessionStorage.setItem('title', title); sessionStorage.setItem('waittime', waittime);  window.open('loader.html', '_self');}</script></head><body><center><h1>9.00 Payloads</h1>";
  int cntr = 0;
  int payloadCount = 0;

#if INTHEN
  payloadCount++;
  cntr++;
  output +=  "<a onclick=\"setpayload('gldhen.bin','GoldHEN v2.0b2','10000')\"><button class=\"btn\">GoldHEN v2.0b2</button></a>&nbsp;";
#endif

  File file = dir.openNextFile();
  while(file){
    String fname = String(file.name());
    if (fname.length() > 0)
    {
      if (fname.endsWith(".gz")) {
        fname = fname.substring(0, fname.length() - 3);
      }
    if (fname.endsWith(".bin"))
    {
      payloadCount++;
      String fnamev = fname;
      fnamev.replace(".bin","");
      output +=  "<a onclick=\"setpayload('" + urlencode(fname) + "','" + fnamev + "','10000')\"><button class=\"btn\">" + fnamev + "</button></a>&nbsp;";
      cntr++;
      if (cntr == 3)
      {
        cntr = 0;
        output +=  "<p></p>";
      }
    }
    }
    file.close();
    file = dir.openNextFile();
  }
  if (payloadCount == 0)
  {
      output += "No .bin payloads found<br>You need to upload the payloads to the ESP32 board.<br>in the arduino ide select <b>Tools</b> &gt; <b>ESP32 Sketch Data Upload</b></center></body></html>";
  }
  output += "</center></body></html>";
  request->send(200, "text/html", output);
}


void handleConfig(AsyncWebServerRequest *request)
{
  if(request->hasParam("ap_ssid", true) && request->hasParam("ap_pass", true) && request->hasParam("web_ip", true) && request->hasParam("subnet", true)) 
  {
    AP_SSID = request->getParam("ap_ssid", true)->value();
    AP_PASS = request->getParam("ap_pass", true)->value();
    String tmpip = request->getParam("web_ip", true)->value();
    String tmpsubn = request->getParam("subnet", true)->value();
    File iniFile = SPIFFS.open("/config.ini", "w");
    if (iniFile) {
    iniFile.print("\r\nSSID=" + AP_SSID + "\r\nPASSWORD=" + AP_PASS + "\r\n\r\nWEBSERVER_IP=" + tmpip + "\r\n\r\nSUBNET_MASK=" + tmpsubn + "\r\n");
    iniFile.close();
    }
    String htmStr = "<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"8; url=/info.html\"><link rel=\"stylesheet\" href=\"style.css\"></head><center><br><br><br><br><br><p id=\"status\"><div id='loader'></div><br>Config saved<br>Rebooting</p></center></html>";
    request->send(200, "text/html", htmStr);
    delay(1000);
    ESP.restart();
  }
  else
  {
    request->redirect("/config.html"); 
  }
}


void handleReboot(AsyncWebServerRequest *request)
{
  //HWSerial.print("Rebooting ESP");
  request->send(200, "text/html", rebootingData);
  delay(1000);
  ESP.restart();
}


void handleConfigHtml(AsyncWebServerRequest *request)
{
  String htmStr = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>Config Editor</title><style type=\"text/css\">body {background-color: #1451AE; color: #ffffff; font-size: 14px; font-weight: bold; margin: 0 0 0 0.0; padding: 0.4em 0.4em 0.4em 0.6em;} input[type=\"submit\"]:hover { background: #ffffff; color: green; }input[type=\"submit\"]:active { outline-color: green; color: green; background: #ffffff; }table {font-family: arial, sans-serif;border-collapse: collapse;}td, th {border: 1px solid #dddddd;text-align: left; padding: 8px;}</style></head><body><form action=\"/config.html\" method=\"post\"><center><table><tr><td>SSID:</td><td><input name=\"ap_ssid\" value=\"" + AP_SSID + "\"></td></tr><tr><td>PASSWORD:</td><td><input name=\"ap_pass\" value=\"" + AP_PASS + "\"></td></tr><tr><td>WEBSERVER IP:</td><td><input name=\"web_ip\" value=\"" + Server_IP.toString() + "\"></td></tr><tr><td>SUBNET MASK:</td><td><input name=\"subnet\" value=\"" + Subnet_Mask.toString() + "\"></td></tr></table><br><input id=\"savecfg\" type=\"submit\" value=\"Save Config\"></center></form></body></html>";
  request->send(200, "text/html", htmStr);
}



void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if(!index){
      String path = request->url();
      if (path != "/upload.html") {
        request->send(500, "text/plain", "Internal Server Error");
        return;
      }
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      if (filename.equals("/config.ini"))
      {return;}
      //HWSerial.printf("Upload Start: %s\n", filename.c_str());
      upFile = SPIFFS.open(filename, "w");
      }
    if(upFile){
        upFile.write(data, len);
    }
    if(final){
        upFile.close();
        //HWSerial.printf("upload Success: %uB\n", index+len);
    }
}


void handleConsoleUpdate(String rgn, AsyncWebServerRequest *request)
{
  String Version = "05.050.000";
  String sVersion = "05.050.000";
  String lblVersion = "5.05";
  String imgSize = "0";
  String imgPath = "";
  String xmlStr = "<?xml version=\"1.0\" ?><update_data_list><region id=\"" + rgn + "\"><force_update><system level0_system_ex_version=\"0\" level0_system_version=\"" + Version + "\" level1_system_ex_version=\"0\" level1_system_version=\"" + Version + "\"/></force_update><system_pup ex_version=\"0\" label=\"" + lblVersion + "\" sdk_version=\"" + sVersion + "\" version=\"" + Version + "\"><update_data update_type=\"full\"><image size=\"" + imgSize + "\">" + imgPath + "</image></update_data></system_pup><recovery_pup type=\"default\"><system_pup ex_version=\"0\" label=\"" + lblVersion + "\" sdk_version=\"" + sVersion + "\" version=\"" + Version + "\"/><image size=\"" + imgSize + "\">" + imgPath + "</image></recovery_pup></region></update_data_list>";
  request->send(200, "text/xml", xmlStr);
}


void handleInfo(AsyncWebServerRequest *request)
{
  float flashFreq = (float)ESP.getFlashChipSpeed() / 1000.0 / 1000.0;
  FlashMode_t ideMode = ESP.getFlashChipMode();
  String output = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>System Information</title><link rel=\"stylesheet\" href=\"style.css\"></head>";
  output += "<hr>###### Software ######<br><br>";
  output += "Firmware version " + firmwareVer + "<br>";
  output += "SDK version: " + String(ESP.getSdkVersion()) + "<br>";
  output += "Chip Id: " + String(ESP.getChipModel()) + "<br><hr>";
  output += "###### CPU ######<br><br>";
  output += "CPU frequency: " + String(ESP.getCpuFreqMHz()) + "MHz<br><hr>";
  output += "###### Flash chip information ######<br><br>";
  output += "Flash chip Id: " +  String(ESP.getFlashChipMode()) + "<br>";
  output += "Estimated Flash size: " + formatBytes(ESP.getFlashChipSize()) + "<br>";
  output += "Flash frequency: " + String(flashFreq) + " MHz<br>";
  output += "Flash write mode: " + String((ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN")) + "<br><hr>";
  output += "###### Sketch information ######<br><br>";
  output += "Sketch hash: " + ESP.getSketchMD5() + "<br>";
  output += "Sketch size: " +  formatBytes(ESP.getSketchSize()) + "<br>";
  output += "Free space available: " +  formatBytes(ESP.getFreeSketchSpace()) + "<br><hr>";
  output += "</html>";
  request->send(200, "text/html", output);
}


void writeConfig()
{
  File iniFile = SPIFFS.open("/config.ini", "w");
  if (iniFile) {
  iniFile.print("\r\nSSID=" + AP_SSID + "\r\nPASSWORD=" + AP_PASS + "\r\n\r\nWEBSERVER_IP=" + Server_IP.toString() + "\r\n\r\nSUBNET_MASK=" + Subnet_Mask.toString() + "\r\n");
  iniFile.close();
  }
}


void setup(){
  //HWSerial.begin(115200);
  //HWSerial.println("Version: " + firmwareVer);
  if (SPIFFS.begin(true)) {
  if (SPIFFS.exists("/config.ini")) {
  File iniFile = SPIFFS.open("/config.ini", "r");
  if (iniFile) {
  String iniData;
    while (iniFile.available()) {
      char chnk = iniFile.read();
      iniData += chnk;
    }
   iniFile.close();
   
   if(instr(iniData,"SSID="))
   {
   AP_SSID = split(iniData,"SSID=","\r\n");
   AP_SSID.trim();
   }
   
   if(instr(iniData,"PASSWORD="))
   {
   AP_PASS = split(iniData,"PASSWORD=","\r\n");
   AP_PASS.trim();
   }
   
   if(instr(iniData,"WEBSERVER_IP="))
   {
    String strwIp = split(iniData,"WEBSERVER_IP=","\r\n");
    strwIp.trim();
    Server_IP.fromString(strwIp);
   }

   if(instr(iniData,"SUBNET_MASK="))
   {
    String strsIp = split(iniData,"SUBNET_MASK=","\r\n");
    strsIp.trim();
    Subnet_Mask.fromString(strsIp);
   }
    }
  }
  else
  {
   writeConfig(); 
  }
  }
  else
  {
    //HWSerial.println("No SPIFFS");
  }


  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(Server_IP, Server_IP, Subnet_Mask);
  WiFi.softAP(AP_SSID.c_str(), AP_PASS.c_str());
  //HWSerial.println("WIFI AP started");

  dnsServer.setTTL(30);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(53, "*", Server_IP);
  //HWSerial.println("DNS server started");


  server.on("/connecttest.txt", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(200, "text/plain", "Microsoft Connect Test");
  });

  server.on("/config.ini", HTTP_ANY, [](AsyncWebServerRequest *request){
   request->send(404);
  });

  server.on("/upload.html", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(200, "text/html", uploadData);
  });

  server.on("/upload.html", HTTP_POST, [](AsyncWebServerRequest *request){
   request->redirect("/fileman.html"); 
  }, handleFileUpload);

  server.on("/fileman.html", HTTP_GET, [](AsyncWebServerRequest *request){
   handleFileMan(request);
  });

  server.on("/delete", HTTP_POST, [](AsyncWebServerRequest *request){
   handleDelete(request);
  });

  server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *request){
   handleConfigHtml(request);
  });
  
  server.on("/config.html", HTTP_POST, [](AsyncWebServerRequest *request){
   handleConfig(request);
  });
  
  server.on("/admin.html", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(200, "text/html", adminData);
  });
  
  server.on("/reboot.html", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(200, "text/html", rebootData);
  });
  
  server.on("/reboot.html", HTTP_POST, [](AsyncWebServerRequest *request){
   handleReboot(request);
  });

  server.on("/update.html", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(200, "text/html", updateData);
  });

  server.on("/update.html", HTTP_POST, [](AsyncWebServerRequest *request){
  }, handleFwUpdate);

  server.on("/info.html", HTTP_GET, [](AsyncWebServerRequest *request){
   handleInfo(request);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send(200, "text/css", styleData);
  });

  server.on("/usbon", HTTP_POST, [](AsyncWebServerRequest *request){
   enableUSB();
   request->send(200, "text/plain", "ok");
  });

  server.on("/usboff", HTTP_POST, [](AsyncWebServerRequest *request){
   disableUSB();
   request->send(200, "text/plain", "ok");
  });
  
#if INTHEN
  server.on("/gldhen.bin", HTTP_GET, [](AsyncWebServerRequest *request){
   AsyncWebServerResponse *response = request->beginResponse_P(200, "application/octet-stream", goldhen, sizeof(goldhen));
   request->send(response);
  });
#endif

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request){
    //HWSerial.println(request->url());
    String path = request->url();
    if (instr(path,"/update/ps4/"))
    {
        String Region = split(path,"/update/ps4/list/","/");
        handleConsoleUpdate(Region, request);
        return;
    }

     if (path.endsWith("index.html") || path.endsWith("index.htm") || path.endsWith("/"))
     {
        request->send(200, "text/html", indexData);
        return;
     }
     if (path.endsWith("style.css"))
     {
        request->send(200, "text/css", styleData);
        return;
     }	 
     if (path.endsWith("payloads.html"))
     {
        handlePayloads(request);
        return;
     }
     if (path.endsWith("loader.html"))
     {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", loader_gz, sizeof(loader_gz));
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
        return;
     }
     if (path.endsWith("usbon"))
     {
        enableUSB();
        request->send(200, "text/plain", "ok");
        return;
     }
     if (path.endsWith("usboff"))
     {
        disableUSB();
        request->send(200, "text/plain", "ok");
        return;
     }

#if INTHEN
  if (path.endsWith("gldhen.bin")){
   AsyncWebServerResponse *response = request->beginResponse_P(200, "application/octet-stream", goldhen, sizeof(goldhen));
   request->send(response);
   return;
  }
#endif

     if (instr(path,"/document/") && instr(path,"/ps4/"))
     {
        path.replace("/document/" + split(path,"/document/","/ps4/") + "/ps4/", "/");
        request->send(SPIFFS, path, getContentType(path));
        return;
     }
    request->send(404);
  });


  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
  //HWSerial.println("HTTP server started");

  

}


static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
  return bufsize;
}


static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
  if (lba > 4){lba = 4;}
  memcpy(buffer, exfathax[lba] + offset, bufsize);
  return bufsize;
}


static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  return true;
}


static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        //HWSerial.println("USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        //HWSerial.println("USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        //HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        //HWSerial.println("USB RESUMED");
        break;
      default:
        break;
    }
  }
}

 
void enableUSB()
{
  USB.onEvent(usbEventCallback);
  dev.vendorID("USB");
  dev.productID("ESP32 Server");
  dev.productRevision("1.0");
  dev.onStartStop(onStartStop);
  dev.onRead(onRead);
  dev.onWrite(onWrite);
  dev.mediaPresent(true);
  dev.begin(8192, 512);
  //USBSerial.begin();
  USB.begin();
  enTime = millis();
  hasEnabled = true;
}


void disableUSB()
{
  enTime = 0;
  hasEnabled = false;
  dev.end();
  //USBSerial.end();
}


void loop(){
   if (hasEnabled && millis() >= (enTime + 15000))
   {
    disableUSB();
   } 
   dnsServer.processNextRequest();
}
