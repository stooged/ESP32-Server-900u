# ESP32 Server 9.00u

<br>

This is a project designed for the <a href=https://www.espressif.com/en/products/socs/esp32-s2>ESP32-S2 boards</a> to provide a wifi http server, dns server and USB storage emulation.

these boards can be used for a plug and play setup(no wiring)<br>

<a href=https://www.wemos.cc/en/latest/s2/s2_mini.html>S2 Mini</a><br>
<a href=https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/hw-reference/esp32s2/user-guide-s2-devkitc-1.html>ESP32-S2-DevKitC-1</a><br>
<a href="http://www.lilygo.cn/prod_view.aspx?TypeId=50063&Id=1320&FId=t3:50063:3">ESP32-S2 ESP32-S2-WOOR</a><br>

<br>

these boards will need a usb A plug wired up to them.<br>
<a href=https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/hw-reference/esp32s2/user-guide-devkitm-1-v1.html>ESP32-S2-DevKitM-1</a> Wiring <a href=https://github.com/stooged/ESP32-Server-900u/blob/main/Images/esp32-s2-devkitm-1.jpg>Diagram</a><br>
<a href=https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/hw-reference/esp32s2/user-guide-saola-1-v1.2.html>ESP32-S2-Saola-1</a> Wiring <a href=https://github.com/stooged/ESP32-Server-900u/blob/main/Images/esp32-s2-saola-1.jpg>Diagram</a><br>

<br><br>

the project is built using <b><a href=https://github.com/me-no-dev/ESPAsyncWebServer>ESPAsyncWebServer</a></b> and <b><a href=https://github.com/me-no-dev/AsyncTCP>AsyncTCP</a></b> so you need to add these libraries to arduino

<a href=https://github.com/me-no-dev/ESPAsyncWebServer>ESPAsyncWebServer</a><br>
<a href=https://github.com/me-no-dev/AsyncTCP>AsyncTCP</a><br>

<br>

install or update the ESP32 core by adding this url to the <a href=https://docs.arduino.cc/learn/starting-guide/cores>Additional Boards Manager URLs</a> section in the arduino "<b>Preferences</b>".

` https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json `

then goto the "<b>Boards Manager</b> and install or update the "<b>esp32</b>" core.

<br>

this is for the 9.00 exploit and the <a href=https://www.espressif.com/en/products/socs/esp32-s2>ESP32 S2</a> boards, it <b>will not work on the original ESP32 boards</b> as they do not support the usb functions required by this project.

you do not need a usb drive with this project as it emulates a usb mass storage device to the console and triggers a filesystem bug to leverage the exploit(exfathax).

the only files required on the spiffs storage of the esp32 are the .bin payloads, everything else is handled internally including generating a list of payloads.

you can still modify the html by uploading your own index.html, if there is no index.html on the spiffs storage the internal pages will be used.

if you have problems compiling the sketch make sure the <a href=https://github.com/espressif/arduino-esp32>ESP32 library</a> is up to date.

the firmware is updatable via http and the payload files can be managed via http.

you can access the main page from the userguide or the consoles webbrowser using any hostname.



<br>
<br>


<b>implemented internal pages</b>

* <b>admin.html</b> - the main landing page for administration.

* <b>index.html</b> - if no index.html is found the server will generate a simple index page and list the payloads automatically.

* <b>info.html</b> - provides information about the esp board.

* <b>upload.html</b> - used to upload files(<b>.bin</b>) to the esp board for the webserver.

* <b>update.html</b> - used to update the firmware on the esp board (<b>fwupdate.bin</b>).

* <b>fileman.html</b> - used to <b>view</b> / <b>download</b> / <b>delete</b> files on the internal storage of the esp board.

* <b>config.html</b> - used to configure wifi ap and ip settings.

* <b>reboot.html</b> - used to reboot the esp board


<br><br>


installation is simple you just use the arduino ide to flash the sketch/firmware to the esp32 board.<br>
<br>
<br>
next you connect to the wifi access point with a pc/laptop, <b>PS4_WEB_AP</b> is the default SSID and <b>password</b> is the default password.<br>
then use a webbrowser and goto http://10.1.1.1/admin.html <b>10.1.1.1</b> is the defult webserver ip.<br>
on the side menu of the admin page select <b>File Uploader</b> and then click <b>Select Files</b> and locate the <b>data</b> folder inside the <b>ESP32_Server_900</b> folder in this repo and select all the files inside the <b>data</b> folder and click <b>Upload Files</b>
you can then goto <b>Config Editor</b> and change the password for the wifi ap.


alternatively if you install this <a href=https://github.com/etherfi/arduino-esp32fs-plugin-esp32s2>plugin</a> to the arduino ide you can upload the files to the esp32-s2 with the arduino ide by selecting <b>Tools > ESP32 Sketch Data Upload</b>

<a href=https://github.com/etherfi/arduino-esp32fs-plugin-esp32s2>Arduino ESP32 filesystem uploader</a>

<img src=https://github.com/stooged/ESP32-Server-900u/blob/main/Images/dataup.jpg><br><br>

the files uploaded using this method are found in the <b>data</b> folder inside the <b>ESP32_Server_900u</b> folder.
