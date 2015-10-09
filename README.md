#HOSTAPD REMOTE CONTROLLER
_ _ _
by Kyewan Ahn (MCLab Kyunghee Univercity)
**C++ Program to make remote configuring hostapd.**

#Dependencies
_ _ _
- [dhcpd]
- [hostapd]
- [c++ boost library(1.59)]

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


```

...ing
