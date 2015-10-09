#include "hostap.h"
#include <boost/format.hpp>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include "pt.h"
#include <iostream>


using namespace std;

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

int hostap::start()
{
	string ifconfBuf;
	ifconfBuf = boost::str(boost::format("ifconfig %s 192.168.1.34") % this->ap_conf.interface);

	system(ifconfBuf.c_str());
	system("dhcpd");

	this->status = "ON";
	return system("hostapd -dd ./openwinnet.conf -B");

}

int hostap::stop()
{
	cout << endl << "Stop HOSTAPD" << endl;
	system("skill dhcpd");
	this->status = "OFF";
	return system("skill hostapd");

}

void hostap::restart()
{
	if(this->status == "ON")
		ap->reboot();
	else
		ap->rebuildConf();
}

void hostap::reboot()
{
	this->stop();
	this->rebuildConf();
	sleep(1);
	this->start();
}

void hostap::rebuildConf()
{
	string buf;
	ofstream out("./temp.conf");
	ifstream in(this->confPath.c_str());

	if(in.is_open() && out.is_open())
	{
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

		while(!in.eof())
		{
			getline(in, buf);
			if(this->isNotConfMember(buf))
				out << buf << endl;
		}
	}

	buf = boost::str(boost::format("rm -rf %s") % this->confPath);
	system(buf.c_str());

	buf = boost::str(boost::format("mv ./temp.conf %s") % this->confPath);
	system(buf.c_str());
	return;


}

void hostap::readConf()
{
	string temp("");

	ifstream in(this->confPath.c_str());
	if(in.is_open())
	{
		while(!in.eof())
		{
			getline(in, temp);
			if( (temp.find("interface=w") != std::string::npos))
			{
				this->ap_conf.interface = temp.substr(10);
			}
			else if(temp.find("bridge") != std::string::npos)
			{
				if(temp[0] == '#')
				{
					this->ap_conf.isBridge = false;
					this->ap_conf.bridge = temp.substr(8);
				}
				else
				{
					this->ap_conf.isBridge = true;
					this->ap_conf.bridge = temp.substr(7);
				}
			}
			else if(temp.find("wpa_passphrase") != std::string::npos)
			{
				if(temp[0] == '#')
				{
					this->ap_conf.isPwd = false;
					this->ap_conf.pwd = temp.substr(16);
				}
				else
				{
					this->ap_conf.isPwd = true;
					this->ap_conf.pwd = temp.substr(15);
				}
			}
			else if(temp.find("ignore_broadcast_ssid") != std::string::npos)
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
			else if(temp.find("ssid") != std::string::npos)
			{
				this->ap_conf.ssid = temp.substr(5);
			}
			else if(temp.find("channel") != std::string::npos)
			{
				this->ap_conf.channel = temp.substr(8);
			}
			else if(temp.find("hw_mode") != std::string::npos)
			{
				this->ap_conf.hw_mode = temp.substr(8);
			}

		}
	}

}

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

void hostap::set_interface(string interface)
{
	this->ap_conf.interface = interface;
}

string hostap::get_interface()
{
	return this->ap_conf.interface;
}

void hostap::set_bridge(string brg)
{
	this->ap_conf.bridge = brg;
}

string hostap::get_bridge()
{
	return this->ap_conf.bridge;
}

void hostap::set_ssid(string ssid)
{
	this->ap_conf.ssid = ssid;
}

string hostap::get_ssid()
{
	return this->ap_conf.ssid;
}

void hostap::set_pwd(string pwd)
{
	this->ap_conf.pwd = pwd;
}

void hostap::pwd_on()
{
	this->ap_conf.isPwd = true;
}

void hostap::pwd_off()
{
	this->ap_conf.isPwd = false;
}

string hostap::get_pwd()
{
	return this->ap_conf.pwd;
}

void hostap::set_hide(string hide)
{
	if(hide == "0")
		this->ap_conf.isHide=false;
	else
		this->ap_conf.isHide=true;
	this->ap_conf.hide = hide;
}

string hostap::get_hide()
{
	return this->ap_conf.hide;
}

void hostap::set_channel(string chl)
{
	this->ap_conf.channel = chl;
}
string hostap::get_channel()
{
	return this->ap_conf.channel;
}

void hostap::set_hwmode(string mode)
{
	this->ap_conf.hw_mode = mode;
}
string hostap::get_hwmode()
{
	return this->ap_conf.hw_mode;
}

string hostap::get_status()
{
	return this->status;
}
void hostap::print()
{
	 std::cout << "interface: " << this->ap_conf.interface << std::endl;
	if(this->ap_conf.isBridge)
		std::cout << "bridge: " << this->ap_conf.bridge << std::endl;
	std::cout << "ssid: " << this->ap_conf.ssid << std::endl;
	std::cout << "channel: " << this->ap_conf.channel << std::endl;
	std::cout << "hw_mode: " << this->ap_conf.hw_mode << std::endl;

}
