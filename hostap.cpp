#include "hostap.h"
#include <boost/format.hpp>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include "protocol_hostap.h"
#include <iostream>


using namespace std;


/*Constructor*/
hostap::hostap(string confpath) : confPath(confpath)
{
	this->status = "OFF";
	this->ap_conf.isBridge = false;
	this->ap_conf.isPwd = false;
	this->ap_conf.isHide = false;
	this->readConf();
}

hostap::hostap(char * confpath) : confPath(confpath)
{
	this->status = "OFF";
	this->ap_conf.isBridge = false;
	this->ap_conf.isPwd = false;
	this->ap_conf.isHide = false;
	this->readConf();
}

/****************************************
 * start method. 			*
 * Starting hostapd using conf file	*
 * **************************************/
int hostap::start()
{
	string ifconfBuf;
	ifconfBuf = boost::str(boost::format("ifconfig %s 192.168.1.34") % this->ap_conf.interface);

	system(ifconfBuf.c_str());
	system("dhcpd");

	this->status = "ON";
	ifconfBuf = boost::str(boost::format("hostapd -dd  %s -B") % this->confPath);
	return system(ifconfBuf.c_str());

}

/****************************
 * stop method. 	    *	
 * **************************/
int hostap::stop()
{
	cout << endl << "Stop HOSTAPD" << endl;
	system("skill dhcpd");
	this->status = "OFF";
	return system("skill hostapd");

}

/************************************************
 * restart method. 				*
 * if hostapd is ruuning, call reboot method,	*
 * else, call rebuildConf method. 		*
 * **********************************************/
void hostap::restart()
{
	if(this->status == "ON")
		this->reboot();
	else
		this->rebuildConf();
}

/****************************************
 * reboot method. 			*
 * call stop, rebuildConf and start	*
 * **************************************/
void hostap::reboot()
{
	this->stop();
	this->rebuildConf();
	sleep(1); 		//for safety running.
	this->start();
}


/*****************************************
 * judge whether input is or not comment * 
 * ***************************************/
bool hostap::isNotComment(string in)
{
	if(in[0] != '#')
		return true;
	else
		return false;
}

/*******************************************************
 * judge whether input is or not ap object's attribute *
 * *****************************************************/
bool hostap::isNotConfMember(string in)
{
	return ((in.find("interface=w") == std::string::npos) &&
		(in.find("bridge") == std::string::npos) &&
		(in.find("ssid") == std::string::npos) &&
		(in.find("channel") == std::string::npos) &&
		(in.find("ignore_broadcast_ssid") == std::string::npos) &&
		(in.find("wpa") == std::string::npos) &&
		(in.find("rsn_pairwise") == std::string::npos) &&
		(in.find("auth_algs") == std::string::npos) &&
		(in.find("hw_mode") == std::string::npos));

}

/*******************************************************************
 * rebuild hostapd conf file					   *
 * first, make temp conf file.					   *
 * second, write object's attribute				   *
 * third, copy the exisiting file execpt object's attribute	   *
 * last, remove the exisiting file, and change the name temp file  *
 * *****************************************************************/
void hostap::rebuildConf()
{
	string buf;
	/*first*/
	ofstream out("./temp.conf");
	ifstream in(this->confPath.c_str());

	if(in.is_open() && out.is_open())
	{
		/*second*/
		buf = boost::str(boost::format("interface=%s") % this->ap_conf.interface);
		out << buf << endl;

		if(this->ap_conf.isBridge)
			buf = boost::str(boost::format("bridge=%s") % this->ap_conf.bridge);
		else
			buf = boost::str(boost::format("#bridge=%s") % this->ap_conf.bridge);
		out << buf << endl;

		buf = boost::str(boost::format("ssid=%s") % this->ap_conf.ssid);
		out << buf << endl;

		buf = boost::str(boost::format("channel=%s") % this->ap_conf.channel);
		out << buf << endl;

		buf = boost::str(boost::format("hw_mode=%s") % this->ap_conf.hw_mode);
		out << buf << endl;

		if(this->ap_conf.isPwd)
		{
			
			buf = boost::str(boost::format("wpa=3\nwpa_passphrase=%s\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP\nrsn_pairwise=CCMP\nauth_algs=1") % this->ap_conf.pwd);
			out << buf << endl;
			
		}
		
		if(this->ap_conf.isHide)
		{
			buf = boost::str(boost::format("ignore_broadcast_ssid=1"));
			out << buf << endl;
		}

		/*third*/
		while(!in.eof())
		{
			getline(in, buf);
			if(this->isNotComment(buf))
			{
				if(this->isNotConfMember(buf))
					out << buf << endl;
			}
		}
	}

	/*last*/
	buf = boost::str(boost::format("rm -rf %s") % this->confPath);
	system(buf.c_str());

	buf = boost::str(boost::format("mv ./temp.conf %s") % this->confPath);
	system(buf.c_str());
	return;


}

/*****************************************
 * read conf file and set ap's attribute *
 * ***************************************/
void hostap::readConf()
{
	string temp("");

	ifstream in(this->confPath.c_str());
	if(in.is_open())
	{
		while(!in.eof())
		{
			getline(in, temp);
			if(this->isNotComment(temp)) //if not comment.
			{
				
				if((temp.find("interface=w") != std::string::npos))			//interface
				{
					this->ap_conf.interface = temp.substr(10);
				}
				else if(temp.find("bridge") != std::string::npos)			//bridge
				{
					this->ap_conf.isBridge = true;
					this->ap_conf.bridge = temp.substr(7);
				}
				else if(temp.find("wpa_passphrase") != std::string::npos)		//password
				{
					this->ap_conf.isPwd = true;
					this->ap_conf.pwd = temp.substr(15);
				}
				else if(temp.find("ignore_broadcast_ssid") != std::string::npos)	//hide
				{
					string tsr = temp.substr(22);
					if(tsr == "0")
					{
						this->ap_conf.isHide = false;
						this->ap_conf.hide = "0";
					}
					else
					{
						this->ap_conf.isHide = true;
						this->ap_conf.hide= "1";
					}
				}
				else if(temp.find("ssid") != std::string::npos)				//ssid
				{
					this->ap_conf.ssid = temp.substr(5);
				}
				else if(temp.find("channel") != std::string::npos)			//channel
				{
					this->ap_conf.channel = temp.substr(8);
				}
				else if(temp.find("hw_mode") != std::string::npos)			//wifi mode
				{
					this->ap_conf.hw_mode = temp.substr(8);
				}
			}

		}
	}

}


/************************
*  setters and getters  *
*************************/
bool hostap::set_interface(string interface)
{
	//check valid param
	if((interface.find("wlan")) != std::string::npos)
	{
		this->ap_conf.interface = interface;
		return true;	
	}
	else
		return false;
}

string hostap::get_interface()
{
	return this->ap_conf.interface;
}

bool hostap::set_bridge(string brg)
{
	//check valid param
	if((brg.find("br")) != std::string::npos)
	{
		this->ap_conf.bridge = brg;
		return true;
	}
	else
		return false;
}

string hostap::get_bridge()
{
	return this->ap_conf.bridge;
}

bool hostap::set_ssid(string ssid)
{
	//check valid param
	int length = ssid.length();
	if(length > 0)
	{
		this->ap_conf.ssid = ssid;
		return true;
	}
	else
		return false;
}

string hostap::get_ssid()
{
	return this->ap_conf.ssid;
}

bool hostap::set_pwd(string pwd)
{
	//check valid param
	int length = pwd.length();
	if(length >= 8)
	{
		this->ap_conf.pwd = pwd;
		return true;
	}
	else
		return false;
}

void hostap::pwd_on()
{
	this->ap_conf.isPwd = true;
}

void hostap::pwd_off()
{
	this->ap_conf.isPwd = false;
	this->ap_conf.pwd = "";
}

string hostap::get_pwd()
{
	return this->ap_conf.pwd;
}

bool hostap::set_hide(string hide)
{
	//check valid param
	if(hide == "0" || hide == "1")
	{
		if(hide == "0")
			this->ap_conf.isHide=false;
		else
			this->ap_conf.isHide=true;
		this->ap_conf.hide = hide;

		return true;
	}
	else
		return false;
}

string hostap::get_hide()
{
	return this->ap_conf.hide;
}

bool hostap::set_channel(string chl)
{
	//check valid param
	int channel = atoi(chl.c_str());
	if( channel > 0 && channel < 12)
	{
		this->ap_conf.channel = chl;
		return true;
	}
	else
		return false;

}

string hostap::get_channel()
{
	return this->ap_conf.channel;
}

bool hostap::set_hwmode(string mode)
{
	//check valid param
	if(mode == "g" || mode =="b")
	{
		this->ap_conf.hw_mode = mode;
		return true;
	}
	else
		return false;
}
string hostap::get_hwmode()
{
	return this->ap_conf.hw_mode;
}
string hostap::get_status()
{
	return this->status;
}

/* print method to debug */
void hostap::print()
{
	 std::cout << "interface: " << this->ap_conf.interface << std::endl;
	if(this->ap_conf.isBridge)
		std::cout << "bridge: " << this->ap_conf.bridge << std::endl;
	std::cout << "ssid: " << this->ap_conf.ssid << std::endl;
	std::cout << "channel: " << this->ap_conf.channel << std::endl;
	std::cout << "hw_mode: " << this->ap_conf.hw_mode << std::endl;

}
