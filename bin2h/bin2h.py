#!/usr/bin/python3
import os
import sys
import gzip
try:filename = sys.argv[1]
except:sys.exit(".bin file required\n\nExample: bin2h goldhen.bin")
f = open(filename, 'rb')
bindat = f.read()
f.close()
f = gzip.open("goldhen.bin.gz", 'wb')
f.write(bindat)
f.close()
cnt = 0
filename = os.path.basename(filename).replace('.bin', '')
tmpdat = "#if INTHEN\n#define INTHEN_NAME \"" + filename + "\"\nstatic const uint8_t goldhen_gz[] PROGMEM = {\n"
with open("goldhen.bin.gz", 'rb') as f:
    chnk = f.read(1)
    while chnk:
        if cnt == 31:
            cnt = 0
            tmpdat = tmpdat + "%s, \n" % ord(chnk)
        else:    
            tmpdat = tmpdat + "%s, " % ord(chnk)
        cnt=cnt+1
        chnk = f.read(1)
tmpdat = tmpdat[:-1] + "\n};\n#endif"
f.close()
if os.path.exists("goldhen.h"):
  os.remove("goldhen.h")
f = open("goldhen.h", 'w+', encoding="utf-8") 
f.write(tmpdat)
f.close()
if os.path.exists("goldhen.bin.gz"):
  os.remove("goldhen.bin.gz")