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
#include "pt.h"
#include "hostap.h"

using boost::asio::ip::tcp;
using namespace std;

const int max_length = 1024;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

hostap* ap;

void process_command(Message& cmd)
{
	switch(cmd.type)
	{
	case START_AP:
		ap->start();
		break;
	case STOP_AP:
		ap->stop();
		break;
	case GET_STATUS:
		break;
	case CHANGE_SSID:
		ap->set_ssid(string(cmd.param));
		ap->restart();
		break;
	case CHANGE_PWD:
		ap->pwd_on();
		ap->set_pwd(string(cmd.param));
		ap->restart();
		break;
	case OFF_PWD:
		ap->pwd_off();
		ap->set_pwd(string(cmd.param));
		ap->restart();
		break;
	case CHANGE_CHANNEL:
		ap->set_channel(string(cmd.param));
		ap->restart();
		break;
	case CHANGE_MODE:
		ap->set_hwmode(string(cmd.param));
		ap->restart();
		break;
	case CHANGE_HIDE:
		ap->set_hide(string(cmd.param));
		ap->restart();
		break;
	default:
		break;
	}
	cmd.type = ACK;
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

void session(socket_ptr sock)
{
	try
	{
		for (;;)
		{
			Message cmd;

			boost::system::error_code error;

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
			ap = new hostap("./openwinnet.conf"); // default confPath
		cout << endl << endl << endl;
		cout << "HostAP Manager Server v0.2" << endl;
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
