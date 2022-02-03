static const char indexData[] PROGMEM = R"==(
<!DOCTYPE html><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Server</title>
<link rel="stylesheet" href="style.css">
</head>
<body>
<div class="plmain">
<iframe src="payloads.html" height="100%" width="100%" frameborder="0"></iframe>
</div>
</body>
</html>
)==";


static const char rebootingData[] PROGMEM = R"==(
<!DOCTYPE html><html>
<head>
<meta http-equiv="refresh" content="8; url=/info.html">
<style type="text/css">#loader {z-index: 1;width: 50px;height: 50px;margin: 0 0 0 0;border: 6px solid #f3f3f3;border-radius: 50%;border-top: 6px solid #3498db;width: 50px;height: 50px;-webkit-animation: spin 2s linear infinite;animation: spin 2s linear infinite; } @-webkit-keyframes spin {0%{-webkit-transform: rotate(0deg);}100%{-webkit-transform: rotate(360deg);}}@keyframes spin{0%{ transform: rotate(0deg);}100%{transform: rotate(360deg);}}
body {background-color: #1451AE; color: #ffffff; font-size: 20px; font-weight: bold; margin: 0 0 0 0.0; padding: 0.4em 0.4em 0.4em 0.6em;}  
#msgfmt { font-size: 16px; font-weight: normal;}
#status {font-size: 16px; font-weight: normal;}
</style>
</head>
<center>
<br><br><br><br><br>
<p id="status"><div id='loader'>
</div><br>Rebooting</p>
</center>
</html>
)==";


static const char updateData[] PROGMEM = R"==(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Firmware Update</title>
<style type="text/css">
#loader {  z-index: 1;  width: 50px;  height: 50px;  margin: 0 0 0 0;  border: 6px solid #f3f3f3;  border-radius: 50%;  border-top: 6px solid #3498db;  width: 50px;  height: 50px;  -webkit-animation: spin 2s linear infinite;  animation: spin 2s linear infinite;}@-webkit-keyframes spin {  0% { -webkit-transform: rotate(0deg); }  100% { -webkit-transform: rotate(360deg); }}@keyframes spin {  0% { transform: rotate(0deg); }  100% { transform: rotate(360deg); }}
body {    background-color: #1451AE; color: #ffffff; font-size: 20px;  font-weight: bold;    margin: 0 0 0 0.0;    padding: 0.4em 0.4em 0.4em 0.6em;}  input[type="submit"]:hover {     background: #ffffff;    color: green; }input[type="submit"]:active {     outline-color: green;    color: green;    background: #ffffff; }input[type="button"]:hover {     background: #ffffff;    color: #000000; }input[type="button"]:active {     outline-color: #000000;    color: #000000;    background: #ffffff; }#selfile {  font-size: 16px;  font-weight: normal;}#status {  font-size: 16px;  font-weight: normal;}
</style>
<script>
function formatBytes(bytes) {  if(bytes == 0) return '0 Bytes';  var k = 1024,  dm = 2,  sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'],  i = Math.floor(Math.log(bytes) / Math.log(k));  return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + ' ' + sizes[i];}
function statusUpl() {  document.getElementById("upload").style.display="none";  document.getElementById("btnsel").style.display="none";  document.getElementById("status").innerHTML = "<div id='loader'></div><br>Uploading firmware file...";  setTimeout(statusUpd, 5000);}
function statusUpd() {  document.getElementById("status").innerHTML = "<div id='loader'></div><br>Updating firmware, Please wait.";}
function FileSelected(e){  file = document.getElementById('fwfile').files[document.getElementById('fwfile').files.length - 1];  if (file.name.toLowerCase() == "fwupdate.bin")  {  var b = file.slice(0, 1);  var r = new FileReader();  r.onloadend = function(e) {  if (e.target.readyState === FileReader.DONE) {  var mb = new Uint8Array(e.target.result);   if (parseInt(mb[0]) == 233)  {  document.getElementById("selfile").innerHTML =  "File: " + file.name + "<br>Size: " + formatBytes(file.size) + "<br>Magic byte: 0x" + parseInt(mb[0]).toString(16).toUpperCase();   document.getElementById("upload").style.display="block"; } else  {  document.getElementById("selfile").innerHTML =  "<font color='#df3840'><b>Invalid firmware file</b></font><br><br>Magic byte is wrong<br>Expected: 0xE9<br>Found: 0x" + parseInt(mb[0]).toString(16).toUpperCase();     document.getElementById("upload").style.display="none";  }    }    };    r.readAsArrayBuffer(b);  }  else  {    document.getElementById("selfile").innerHTML =  "<font color='#df3840'><b>Invalid firmware file</b></font><br><br>File should be fwupdate.bin";    document.getElementById("upload").style.display="none";  }}
</script>
</head>
<body>
<center>
<form action="/update.html" enctype="multipart/form-data" method="post"><p>Firmware Updater<br><br></p><p>
<input id="btnsel" type="button" onclick="document.getElementById('fwfile').click()" value="Select file" style="display: block;">
<p id="selfile"></p>
<input id="fwfile" type="file" name="fwupdate" size="0" accept=".bin" onChange="FileSelected();" style="width:0; height:0;"></p><div><p id="status"></p>
<input id="upload" type="submit" value="Update Firmware" onClick="statusUpl();" style="display: none;"></div></form><center></body></html>
)==";


static const char uploadData[] PROGMEM = R"==(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>File Upload</title>
<style type="text/css">
#loader {  z-index: 1;  width: 50px;  height: 50px;  margin: 0 0 0 0;  border: 6px solid #f3f3f3;  border-radius: 50%;  border-top: 6px solid #3498db;  width: 50px;  height: 50px;  -webkit-animation: spin 2s linear infinite;  animation: spin 2s linear infinite;}@-webkit-keyframes spin {  0% { -webkit-transform: rotate(0deg); }  100% { -webkit-transform: rotate(360deg); }}@keyframes spin {  0% { transform: rotate(0deg); }  100% { transform: rotate(360deg); }}body {    background-color: #1451AE; color: #ffffff; font-size: 20px;  font-weight: bold;    margin: 0 0 0 0.0;    padding: 0.4em 0.4em 0.4em 0.6em;}  input[type="submit"]:hover {     background: #ffffff;    color: green; }input[type="submit"]:active {     outline-color: green;    color: green;    background: #ffffff;  } input[type="button"]:hover {     background: #ffffff;    color: #000000; }input[type="button"]:active {     outline-color: #000000;    color: #000000;    background: #ffffff; }#selfile {  font-size: 16px;  font-weight: normal;}#status {  font-size: 16px;  font-weight: normal;}
</style>
<script>
function formatBytes(bytes) {  if(bytes == 0) return '0 Bytes';  var k = 1024,  dm = 2,  sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'],  i = Math.floor(Math.log(bytes) / Math.log(k));  return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + ' ' + sizes[i];}
function statusUpl() {  document.getElementById("upload").style.display="none";  document.getElementById("btnsel").style.display="none";  document.getElementById("status").innerHTML = "<div id='loader'></div><br>Uploading files";}
function FileSelected(e){  var strdisp = "";  var file = document.getElementById("upfiles").files;  for (var i = 0; i < file.length; i++)  {   strdisp = strdisp + file[i].name + " - " + formatBytes(file[i].size) + "<br>";  }  document.getElementById("selfile").innerHTML = strdisp;  document.getElementById("upload").style.display="block";}
</script>
</head>
<body>
<center>
<form action="/upload.html" enctype="multipart/form-data" method="post">
<p>File Uploader<br><br></p><p>
<input id="btnsel" type="button" onclick="document.getElementById('upfiles').click()" value="Select files" style="display: block;"><p id="selfile"></p>
<input id="upfiles" type="file" name="fwupdate" size="0" onChange="FileSelected();" style="width:0; height:0;" multiple></p><div><p id="status"></p>
<input id="upload" type="submit" value="Upload Files" onClick="statusUpl();" style="display: none;"></div>
</form><center>
</body></html>
)==";


static const char adminData[] PROGMEM = R"==(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Admin Panel</title>
<link rel="stylesheet" href="style.css">
</head>
<body><div class="sidenav"><a href="/index.html" target="mframe">Main Page</a>
<a href="/info.html" target="mframe">ESP Information</a>
<a href="/fileman.html" target="mframe">File Manager</a>
<a href="/upload.html" target="mframe">File Uploader</a>
<a href="/update.html" target="mframe">Firmware Update</a>
<a href="/config.html" target="mframe">Config Editor</a>
<a href="/reboot.html" target="mframe">Reboot ESP</a>
</div>
<div class="main">
<iframe src="info.html" name="mframe" height="100%" width="100%" frameborder="0"></iframe>
</div>
</table>
</body></html> 
)==";


static const char rebootData[] PROGMEM = R"==(
<!DOCTYPE html><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP Reboot</title>
<link rel="stylesheet" href="style.css">
<script>
function statusRbt() { var answer = confirm("Are you sure you want to reboot?");  if (answer) {document.getElementById("reboot").style.display="none";   document.getElementById("status").innerHTML = "<div id='loader'></div><br>Rebooting ESP Board"; return true;  }else {   return false;  }}
</script>
</head>
<body>
<center>
<form action="/reboot.html" method="post">
<p>ESP Reboot<br><br></p>
<p id="msgrbt">This will reboot the esp board</p><div>
<p id="status"></p>
<input id="reboot" type="submit" value="Reboot ESP" onClick="return statusRbt();" style="display: block;"></div>
</form><center>
</body></html>
)==";


static const char autohenData[] PROGMEM = R"==(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP Server</title>
<script>
function setpayload(payload,title,waittime)
{
   sessionStorage.setItem('payload', payload);
   sessionStorage.setItem('title', title);
   sessionStorage.setItem('waittime', waittime);
   window.open("loader.html", "_self");
}
</script>
<link rel="stylesheet" href="style.css">
</head>
<body onload="setpayload('gldhen.bin','GoldHEN','12000');">
</body>
</html>
)==";


static const char styleData[] PROGMEM = R"==(
body {
background-color: #1451AE;
color: #ffffff;
font-size: 14px;
font-weight: bold;
margin: 0 0 0 0.0;
overflow-y:hidden;
padding: 0.4em 0.4em 0.4em 0.6em;
} 

.btn {
background-color: DodgerBlue;
border: none;
color: white;
padding: 12px 16px;
font-size: 16px;
cursor: pointer;
font-weight: bold;
}

.btn:hover {
background-color: RoyalBlue;
}

.main {
margin-left: 150px;     
padding: 10px 10px; 
position: absolute; 
top: 0; 
right: 0;
bottom: 0; 
left: 0;
overflow-y:hidden;
}

.plmain {
  padding: 0px 0px;
  position: absolute; 
  top: 0; 
  right: 0;
  bottom: 0; 
  left: 0;
  overflow-y:hidden;
}

a:link {
color: #ffffff; 
text-decoration: none;
} 

a:visited {
color: #ffffff; 
text-decoration: none;
} 

a:hover {
color: #ffffff; 
text-decoration: underline;
} 

a:active {
color: #ffffff; 
text-decoration: underline;
} 

table {
font-family: arial, sans-serif; 
border-collapse: collapse; 
width: 100%;
} 

td, th {
border: 1px solid #dddddd; text-align: left; padding: 8px;
} 


input[type="submit"]:hover { 
background: #ffffff;
color: green; 
}

input[type="submit"]:active { 
outline-color: green;
color: green;
background: #ffffff; 
}

#selfile {  
font-size: 16px;  
font-weight: normal;
}

.sidenav {
width: 140px;
position: fixed;
z-index: 1;
top: 20px;
left: 10px;
background: #6495ED;
overflow-x: hidden;
padding: 8px 0;
}

.sidenav a {
padding: 6px 8px 6px 16px;
text-decoration: none;
font-size: 14px;
color: #ffffff;
display: block;
}

.sidenav a:hover {
color: #1451AE;
}

msg {
color: #ffffff; 
font-weight: 
normal; 
text-shadow: none;
}
)==";
