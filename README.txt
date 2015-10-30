Title: HTTP Web Server using the Light Weight IP (lwIP) Stack

Description:

A simple http web server running on port 80 using the Intel Galileo Gen 2 
on-board GMAC Ethernet device and the lwIP stack.

The IP address used for the GMAC Ethernet device can be configured as static or
assigned dynamically. The default is to assign an address dynamically via DHCP.
The assigned IP address is displayed on the Grove - Starter Kit LCD display
as well as on the serial console.

--------------------------------------------------------------------------------

Sample Output on serial console:

Application is now running
Adding gmac
Initialize LCD ... done  
Link is down
Link is up
IP address: 10.0.0.2 - dhcp

Web Server URL (mouse ctrl-click): http://10.0.0.2


Sample Output on LCD when Link is up (line 1 and line 2):

IP ADDR: dhcp
10.0.0.2

Sample Output on LCD when Link is down (line 1):

Link is down

Sample Output from Web Browser access to the assigned IP address port 80:

Web content from [ httpserver_raw/fs/index.html ]

