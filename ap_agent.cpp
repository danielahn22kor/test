/********************
 @file ap_agent.cpp
 @date 2015/10/11
 @author 안계완
 @brief processing message from mananger program
*********************/

#include <cstdlib>
#include <cstring>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include "protocol_hostap.h"
#include "hostap.h"

using boost::asio::ip::tcp;
using namespace std;

const int max_length = 1024;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

hostap* ap;			//AP object to control hostap


/******************************
 * in: cmd MessageType
 * out: cmd.type, cmd.param will be changed
 * des: Processing Command by cmd.type
 * ****************************/
void process_command(Message& cmd)
{
	bool isSuccess = true;
	string errorM = "";
	/**************************
	 * process command by cmd.type
	 * if there is error, ack messase will be error message
	 * else, ack message will be status 
	 * ************************/
	switch(cmd.type)
	{
	case START_AP:
		if(ap->get_status() == "OFF")
			ap->start();
		else
		{
			isSuccess = false;
			errorM = "hostapd already is running\n";
		}
		break;
	case STOP_AP:
		if(ap->get_status() == "ON")
			ap->stop();
		else
		{
			isSuccess = false;
			errorM = "hostapd is not running\n";
		}
		break;
	case GET_STATUS:
		break;
	case CHANGE_SSID:
		
		if(isSuccess = ap->set_ssid(string(cmd.param)))
			ap->restart();
		else
			errorM = "SSID must be over 1 character\n";
		break;
	case CHANGE_PWD:
		if(isSuccess = ap->set_pwd(string(cmd.param)))
		{
			ap->pwd_on();
			ap->restart();
		}
		else
			errorM = "Password must be over 8 characters\n";
		break;
	case OFF_PWD:
		ap->pwd_off();
		ap->restart();
		break;
	case CHANGE_CHANNEL:
		if(isSuccess = ap->set_channel(string(cmd.param)))
			ap->restart();
		else
			errorM = "Channel must be in range ( 1 ... 11 )\n";
		break;
	case CHANGE_MODE:
		if(isSuccess = ap->set_hwmode(string(cmd.param)))
			ap->restart();
		else
			errorM = "Mode must be 'g' or 'b'\n";
		break;
	case CHANGE_HIDE:
		ap->set_hide(string(cmd.param));
		ap->restart();
		break;
	default:
		break;
	}

	cmd.type = ACK;
	//if Error is not occured, make status message.
	if(isSuccess)
	{
		sprintf(cmd.param, "\nSTATUS: %s\nSSID: %s\nCHANNEL: %s\nMODE: %s\n", (ap->get_status()).c_str(), (ap->get_ssid()).c_str(), (ap->get_channel()).c_str(), (ap->get_hwmode()).c_str());

		if(ap->isPwd())
		{
			string temp;
			temp = boost::str(boost::format("password: %s\n") % ap->get_pwd());

			strcat(cmd.param, temp.c_str());
		}
		if(ap->isHide())
			strcat(cmd.param, "Hide: true\n");
	}
	else
		strcpy(cmd.param, errorM.c_str());
}

void session(socket_ptr sock)
{
	try
	{
		for (;;)
		{
			Message cmd;

			boost::system::error_code error;

			//read message.
			size_t length = boost::asio::read(*sock, boost::asio::buffer((char *)&cmd, sizeof(cmd)));
			if (error == boost::asio::error::eof || length != sizeof(cmd))
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.
			if(cmd.type == DISCONNECT)
			{
				(*sock).close();
				break;
			}
			
			process_command(cmd);
			
			length = sizeof(cmd);	
			boost::asio::write(*sock, boost::asio::buffer((char *)&cmd, length));

			//initialize buf
			bzero(&cmd, sizeof(cmd));

		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

void server(boost::asio::io_service& io_service, unsigned short port)
{
	tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
	for (;;)
	{
		socket_ptr sock(new tcp::socket(io_service));
		a.accept(*sock);
		boost::thread t(boost::bind(session, sock));
	}
}

void closeServer(int signum)
{
	//if occur SIGINT, kill dhcpd, hostapd
	system("skill dhcpd");
	system("skill hostapd");
	exit(0);
}

int main(int argc, char* argv[])
{
	try
	{
		signal(SIGINT, closeServer);

		//exception handling
		if (argc > 2)
		{
			std::cerr << "Usage: " << argv[0] << " [confPath]\n";
			return 1;
		}

		//manual conf path
		if(argc == 2)
			ap = new hostap(argv[1]);
		else
			ap = new hostap("./conf/openwinnet.conf"); // default confPath
		cout << endl << endl << endl;
		cout << "HostAP Manager Server v0.3" << endl;
		cout << "Copyright 2015 Kyung Hee University Mobile Convergence Lab" << endl;
		cout << "All rights reserved." << endl;
		cout << "For info, please contact to roy1022@hanamil.net " << endl << endl << endl;

		
		boost::asio::io_service io_service; //asio tcp service

		server(io_service, AGENT_PORT); // server open
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
