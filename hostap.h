#ifndef __HOSTAP_H__
#define __HOSTAP_H__

/**
 @file hostap.h
 @date 2015/09/28
 @author 안계완
**/



#include "pt.h"
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

/**
 @class hostap
 @date 2015/09/28
 @author 안계완
 @brief abstraction class of hostap for easily using hostapd
 @warning not yet exist exception handling 

**/

class hostap
{
public:
	hostap(string confPath);	//Constructor using conf path(string)
	hostap(char * confPath);	//Constructor using conf path(char *)
	int start();			//Start hostapd 
	int stop();			//Stop hostapd
	void restart();			//Restart hostapd
	void rebuildConf();		//Remake(rebuild) hostapd's conf file
	void readConf();		//Read conf file and save at ap_conf
	bool isNotConfMember(string in);//Boolean of ap_config structer's member
	void print();			//Print all ap_config structure's member
	
	/**
	 setter and getter
	**/
	void set_interface(string interface); 
	string get_interface();

	void set_bridge(string brg);
	string get_bridge();

	void set_ssid(string ssid);
	string get_ssid();

	void set_channel(string chl);
	string get_channel();

	void pwd_on();
	void pwd_off();

	bool isPwd() { return this->ap_conf.isPwd; }

	void set_pwd(string pwd);
	string get_pwd();

	void set_hide(string hide);
	string get_hide();
	bool isHide() { return this->ap_conf.isHide; } 

	void set_hwmode(string mode);
	string get_hwmode();

	string get_status();

private:
	ap_config ap_conf;		//Object for save a config info
	string confPath;		//conffile's path
	string status;
};


#endif
