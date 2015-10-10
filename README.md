#HOSTAPD REMOTE CONTROLLER
_ _ _
by Kyewan Ahn (MCLab Kyunghee Univercity)


**C++ Program to make remote configuring hostapd.**

#Dependencies
_ _ _
- [dhcpd]
- [hostapd]
- [c++ boost library(1.59)]

#Before Started
_ _ _

**YOU MUST INSTALL and CONFIGURE dhcpd, hostapd + Activate MASQUERADING**


OPEN /etc/sysctl.conf file

`net.ipv4.ip_forward=1`

OPEN /etc/rc.local
```
/sbin/iptables -P FORWARD ACCEPT
/sbin/iptables --table nat -A POSTROUTING -o (backbone interface ex: eth0) -j MASQUERADE
```

INSTALL isc-dhcp-server, hostapd, bridge-utils, iw 

`UBUNTU : $ apt-get install bridge-utils iw hostapd isc-dhcp-server`


configure dhcp server interface. open /etc/default/isc-dhcp-server 

`INTERFACES=(ap interface, ex: "wlan0")`

#Usage
_ _ _
##### **Before using this, You must check /etc/dhcpd/dhcpd.conf file. Default dhcp router option: 192.168.1.34, Referrence dhcpd.conf file**

If your OS is not **ubuntu 14.04**, you have to build boost lib and change the libraries(*.a).([check this page](http://www.boost.org/doc/libs/1_59_0/more/getting_started/index.html))


You build this program using 'make'
So, you must check 'makefile'( change the path of boost )

`$ make`

manager and ap_agent requires root privileges to work

`$ chmod +x ap_agent, privileges`



ap_agent will be control ap's hostapd.

`Usage: ap_agent [hostapd conffile path]`

manager will be remote control to ap.

`Usage: manager [ap's ip]`

```
Usage:
Commands: 
  help or h            show this usage help    
  start                start hostap            
  stop                 stop hostap             
  status               show ap's status        
  ssid <params>        change ssid             
  password [params]    change password(if params blank, off password) 
  channel <params>     change channel          
  mode <g,b>           change mode             
  hide <on, off>       broadcast on, off       
  clear or cl          clear line              
  quit                 exit this program   

```

...ing
