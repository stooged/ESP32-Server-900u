# ESP32 Partition Modification


this is for the 16MB <a href=https://feathers2.io/>FeatherS2</a> board with 16mb of flash, it will do nothing for other boards.

the <a href=https://github.com/espressif/arduino-esp32>ESP32 library</a> does not have support for large spiffs partitions so i have created a few tables to enable large spiffs on the 16mb feather board.

<br>

to install

place the contents of the <a href=https://github.com/stooged/ESP32-Server-900u/tree/main/esplib_mod/partitions>partitions</a> folder inside the following folder

`C:\Users\***PCUSERNAME**\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.2\tools\partitions\`



place the contents of the <a href=https://github.com/stooged/ESP32-Server-900u/tree/main/esplib_mod/2.0.2>2.0.2</a> folder inside the following folder and select yes to overwrite 

`C:\Users\***PCUSERNAME**\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.2\`

<br>

the following partiton schemes will be added.

16M Flash (1MB APP/13MB SPIFFS)<br>
16M Flash (2MB APP/11MB SPIFFS)<br>
16M Flash (3MB APP/9MB SPIFFS)<br>


16M Flash No OTA (1MB APP/14MB SPIFFS)<br>
16M Flash No OTA (2MB APP/13MB SPIFFS)<br>
16M Flash No OTA (3MB APP/12MB SPIFFS)<br>

the <b>No OTA</b> partitions disable the ability to update the firmware over the air which means if you want to update the board you will need to plug it into a pc and flash the updates.





