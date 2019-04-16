#!/usr/bin/env python3
 
# A simple web server in Python
 
import socket
import re
import signal
import sys

def ctrlc_handler(signal, frame):
    print('You pressed Ctrl+C!')
    sys.exit(0)
        
def return_error_page(sockfile, errornum, errormsg):
      sockfile.write("""HTTP/1.0 {0} {1}
Content-Type: text/html

<html>
<head>
<title>{1}</title>
</head>
<body>
{1}
</body>
</html>
  """.format(errornum, errormsg))

signal.signal(signal.SIGINT, ctrlc_handler)
 
# Standard socket stuff:
if len(sys.argv) == 2:
  port = int(sys.argv[1])
else:
  port = 8080
  
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', port))
sock.listen(1)  # don't queue up any requests

workersock = 0

try: 
  # Loop forever, listening for requests:
  while True:
      print("Web server active on port " + str(port) + ", waiting for connection...")
      workersock, caddr = sock.accept()
      print("Connection from: " + str(caddr))
      
      with workersock.makefile('rw', 1024) as sockfile:
        
        req = sockfile.readline().strip()  # get the request, 1kB max
        print(req)
        
        # Read rest of request headers
        while sockfile.readline().strip():
            pass

        # parse URI
        match = re.match('^GET (.+) HTTP/1.[01]$', req)
        if match:
            filename = match.group(1)
            print("Path requested: " + filename + "\n")
            try:
              with open(filename, "r") as myfile:
                sockfile.write("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n")
                for line in myfile:
                  sockfile.write(line)
            except IOError:
              return_error_page(sockfile, 404, "Not found")
        else:
            # If not a valid URI return a 400 (page not found)
            return_error_page(sockfile, 400, "Bad request")
            
      workersock.close()
finally:
  print("Shutting down...")
  if workersock:
    workersock.close()  
  sock.close()
  
  