#if USBCONTROL | defined(CONFIG_IDF_TARGET_ESP32S2) | defined(CONFIG_IDF_TARGET_ESP32S3)
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
#else
static const char indexData[] PROGMEM = R"==(
<!DOCTYPE html>
<html manifest="cache.manifest">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Server</title>
<link rel="stylesheet" href="style.css">
</head>
<body>
<div id="progpanel"></div>
</body>
<script>

  function loadMenu(e) {
    window.location.href = 'menu.html';
  }
  
  function loadProg(e) {
    var htmstr = ['<div id=\"prog\" class=\"progbar\"><span id=\"progper\">0%</span><div id=\"progani\"></div><span class=\"proglbl\">Caching Exploit Files...</span></div>'];
  var progPanel = document.getElementById('progpanel');
  progPanel.innerHTML = htmstr;
  }
  
    function progEvent(e) {
  var progWidth = Math.round(Number(e.loaded + 1) / Number(e.total + 1) * 100);
  document.getElementById('progper').innerHTML = progWidth - 1 +'%';
  document.getElementById('progani').style.width = progWidth - 1 +'%';
  }
  
   window.applicationCache.addEventListener('downloading', loadProg, false);
   window.applicationCache.addEventListener('cached',  loadMenu, false);
   window.applicationCache.addEventListener('noupdate',  loadMenu, false);
   window.applicationCache.addEventListener('error',  loadMenu, false);
   window.applicationCache.addEventListener('progress', progEvent, false);
</script>
</html>
)==";


static const char menuData[] PROGMEM = R"==(
<!DOCTYPE html><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Server</title>
<link rel="stylesheet" href="style.css">
<script>
if (window.location.pathname.startsWith("/document/"))
{
  var docpath = window.location.pathname.replace("menu.html","index.html");
  window.addEventListener('popstate', (event) => {
  alert("Close Browser");
  history.pushState({}, '', docpath);
  },false);
  history.pushState({}, '', docpath);
}else{
  window.addEventListener('popstate', (event) => {
  alert("Close Browser");
  history.pushState({}, '', '/');
  },false);
  history.pushState({}, '', '/');
}
</script>
</head>
<body>
<div class="plmain">
<iframe src="payloads.html" height="100%" width="100%" frameborder="0"></iframe>
</div>
</body>
</html>
)==";
#endif

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
<link rel="stylesheet" href="style.css">
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
<link rel="stylesheet" href="style.css">
<style>body{overflow-y:auto;}</style>
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


#if USECONFIG
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
<a href="/format.html" target="mframe">Storage Format</a>
<a href="/reboot.html" target="mframe">Reboot ESP</a>
</div>
<div class="main">
<iframe src="info.html" name="mframe" height="100%" width="100%" frameborder="0"></iframe>
</div>
</table>
</body></html> 
)==";
#else
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
<a href="/format.html" target="mframe">Storage Format</a>
<a href="/reboot.html" target="mframe">Reboot ESP</a>
</div>
<div class="main">
<iframe src="info.html" name="mframe" height="100%" width="100%" frameborder="0"></iframe>
</div>
</table>
</body></html> 
)==";
#endif

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


static const char formatData[] PROGMEM = R"==(
<!DOCTYPE html><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Storage Format</title>
<link rel="stylesheet" href="style.css">
<script>function statusFmt() { 
var answer = confirm("Are you sure you want to format?"); 
if (answer) {
document.getElementById("format").style.display="none"; 
document.getElementById("status").innerHTML = "<div id='loader'></div><br>Formatting Storage"; 
setTimeout(function(){ window.location.href='/fileman.html'; }, 8000);
return true;
} else {
return false;
}}</script>
</head>
<body><center>
<form action="/format.html" method="post">
<p>Storage Format<br><br>
</p><p id="msgfmt">This will delete all the files on the server</p><div><p id="status"></p>
<input id="format" type="submit" value="Format Storage" onClick="return statusFmt();" style="display: block;"></div></form>
<center></body></html>
)==";


#if AUTOHEN
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
<body onload="setpayload('goldhen.bin','GoldHEN','12000');">
</body>
</html>
)==";
#endif



#if !USBCONTROL && defined(CONFIG_IDF_TARGET_ESP32)
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
transition-duration: 0.4s;
box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);
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

input[type="button"]:hover {
background: #ffffff;
color: #000000;
}

input[type="button"]:active {
outline-color: #000000;
color: #000000;
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

#prog {
-webkit-background-clip: padding-box;
background-clip: padding-box;
-webkit-pointer-events: none;
pointer-events: none;
-webkit-user-select: none;
user-select: none;
z-index: 2000;
position: fixed;
margin: auto;
top: 12px;
left: 0;
right: 0;
bottom: 0;
width: 870px;
height: 60px;
border: 3px solid #fff;
}
  
#prog span {
background-color: 1451AE;
position: absolute;
width: 100%;
display: block;
text-align: center;
font-size: 25px;
color: #ffffff;
font-weight: bold;
line-height: 60px;
}

#prog span#progper {
bottom: 100%;
}

#prog span.proglbl {
top: 100%;
text-transform: uppercase;
}

#prog #progani {
-webkit-background-clip: padding-box;
background-clip: padding-box;
-webkit-transition-property: width;
transition-property: width;
-webkit-transition-duration: 0.125s;
transition-duration: 0.125s;
width: 0;
min-width: 5px;
max-width: 864px;
z-index: 2000;
display: block;
position: absolute;
left: 0;
top: 0;
height: 60px;
-webkit-background-size: 54px 54px;
background-size: 54px 54px;
background-image: 
-webkit-linear-gradient(45deg, transparent 33%, rgba(0, 0, 0, .1) 33%,  rgba(0,0, 0, .1) 66%, transparent 66%),
-webkit-linear-gradient(top, rgba(30, 144, 255, 0.9),  rgba(30, 144, 255, 0.9)),
-webkit-linear-gradient(left,rgba(30, 144, 255, 0.9), rgba(30, 144, 255, 0.9));
border-radius: 2px; 
background-size: 5px 2px, 100% 100%, 100% 100%;
-webkit-animation: animate-stripes 10s linear infinite;
animation: animate-stripes 10s linear infinite;
color: DodgerBlue;
}

@-webkit-keyframes animate-stripes {
100% { background-position: 100px 0px; }
}

@keyframes animate-stripes {
100% { background-position: 100px 0px; }
}

#loader {  
z-index: 1;  
width: 50px;  
height: 50px;  
margin: 0 0 0 0;  
border: 6px solid #f3f3f3;  
border-radius: 50%;  
border-top: 6px solid #3498db;  
width: 50px;  
height: 50px;  
-webkit-animation: spin 2s linear infinite;  
animation: spin 2s linear infinite;
}
  
@-webkit-keyframes spin {
0%{ -webkit-transform: rotate(0deg); }
100% { -webkit-transform: rotate(360deg); }
}

@keyframes spin {
0% { transform: rotate(0deg); }
100% {transform: rotate(360deg); }
}

#status {
font-size: 16px;
font-weight: normal;
}

#msgfmt {
font-size: 16px;  
font-weight: normal;
}
)==";
#else
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
transition-duration: 0.4s;
box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);
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

.slct  {
transition-duration: 0.4s;
box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);
text-align: center;
-webkit-appearance: none;
background-color: DodgerBlue;
border: none;
color: white;
padding: 9px 1px;
font-size: 16px;
cursor: pointer;
font-weight: bold;
}

.slct:hover {
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

input[type="button"]:hover {
background: #ffffff;
color: #000000;
}

input[type="button"]:active {
outline-color: #000000;
color: #000000;
background: #ffffff;
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

#loader {  
z-index: 1;  
width: 50px;  
height: 50px;  
margin: 0 0 0 0;  
border: 6px solid #f3f3f3;  
border-radius: 50%;  
border-top: 6px solid #3498db;  
width: 50px;  
height: 50px;  
-webkit-animation: spin 2s linear infinite;  
animation: spin 2s linear infinite;
}
  
@-webkit-keyframes spin {
0%{ -webkit-transform: rotate(0deg); }
100% { -webkit-transform: rotate(360deg); }
}

@keyframes spin {
0% { transform: rotate(0deg); }
100% {transform: rotate(360deg); }
}

#status {
font-size: 16px;
font-weight: normal;
}

#msgfmt {
font-size: 16px;  
font-weight: normal;
}
)==";
#endif
