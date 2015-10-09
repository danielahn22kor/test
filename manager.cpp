#include <cstdlib>
#include <cstring>
#include <string>
#include <signal.h>
#include <iostream>
#include <iomanip>
#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#include "protocol_hostap.h"

using boost::asio::ip::tcp;

using namespace std;
enum { max_length = 1024 };

tcp::socket* s;
void disconnect(int signum)
{
	Message cmd;
	cmd.type = DISCONNECT;
	strcpy(cmd.param, "");
	size_t cmd_size = sizeof(cmd);
	boost::asio::write(*s, boost::asio::buffer((char *)&cmd, cmd_size));
	s->close();
	exit(0);
}

void help()
{
	cout << "Commands: "<< endl;
	
	cout <<"  " << setw(20) << left << "help or h" << setw(25) << setfill(' ') << " show this usage help" << endl;

	cout <<"  " << setw(20) << left << "start" << setw(25) << setfill(' ') << " start hostap" << endl;

	cout <<"  " << setw(20) << left << "stop" << setw(25) << setfill(' ') << " stop hostap" << endl;

	cout <<"  " << setw(20) << left << "status" << setw(25) << setfill(' ') << " show ap's status" << endl;

	cout <<"  " << setw(20) << left << "ssid <params>" << setw(25) << setfill(' ') << " change ssid " << endl;

	cout <<"  " << setw(20) << left << "password [params]" << setw(25) << setfill(' ') << " change password(if params blank, off password) " << endl;

	cout <<"  " << setw(20) << left << "channel <params>" << setw(25) << setfill(' ') << " change channel " << endl;

	cout <<"  " << setw(20) << left << "mode <g,n>" << setw(25) << setfill(' ') << " change mode " << endl;

	cout <<"  " << setw(20) << left << "hide <on, off>" << setw(25) << setfill(' ') << " broadcast on, off " << endl;

	cout <<"  " << setw(20) << left << "clear or cl" << setw(25) << setfill(' ') << " clear line" << endl;

	cout <<"  " << setw(20) << left << "quit" << setw(25) << setfill(' ') << " exit this program " << endl;

}

Message& parse_CMD(string str)
{
	using namespace std;
	Message cmd;
	/**
		parsing command 
		Usage:
		 start
		 stop
		 status
		 ssid <param>
		 password <param>
		 channel <1~11>
		 mode <g, n>
		 hide <on, off>
		 clear
	**/
	if((str == "start") && (str.length() == 5))
	{
		cmd.type = START_AP;
		strcpy(cmd.param ,"");
	}
	else if((str == "stop") && (str.length() == 4))
	{
		cmd.type = STOP_AP;
		strcpy(cmd.param ,"");
	}
	else if((str == "status") && (str.length() == 6))
	{
		cmd.type = GET_STATUS;
		strcpy(cmd.param ,"");
	}
	else if(str.find("ssid") != std::string::npos)
	{
		cmd.type = CHANGE_SSID;
		strcpy(cmd.param ,(str.substr(5)).c_str());
	}
	else if(str.find("password") != std::string::npos)
	{
		if(str.length() == 8)
		{
			cmd.type = OFF_PWD;
			strcpy(cmd.param ,"");
		}
		else
		{
			string paswd = str.substr(9);

			if(paswd.length() < 8)
			{
				cout << "Password's length must be between 8 and 63" << endl;
				cmd.type = mNULL;

			}
			else
			{
				cmd.type = CHANGE_PWD;
				strcpy(cmd.param ,paswd.c_str());
			}
		}
	}
	else if(str.find("channel") != std::string::npos)
	{
		cmd.type = CHANGE_CHANNEL;
		strcpy(cmd.param ,(str.substr(8)).c_str());
	}
	else if(str.find("hide") != std::string::npos)
	{
		string temp;
		temp = str.substr(5);

		if(temp == "ON" || temp == "OFF" || temp == "on" || temp == "off")
		{
			cmd.type = CHANGE_HIDE;
			if(temp == "ON" || temp == "on")
				strcpy(cmd.param, "1");
			else
				strcpy(cmd.param, "0");
		}
		else
		{
			cmd.type = mNULL;
			cout << "Invalid Request, Please See help" << endl;
		}
	}
	else if(str.find("mode") != std::string::npos)
	{
		cmd.type = CHANGE_MODE;
		strcpy(cmd.param ,(str.substr(5)).c_str());
	}
	else if(str == "quit" && (str.length() == 4))
	{
		disconnect(SIGINT);
	}
	else if((str == "help" && (str.length() == 4)) || (str == "h" && (str.length() == 1)) )
	{
		cmd.type = mNULL;
		help();
	}
	else if((str == "clear" && (str.length() == 5)) || (str == "cl" && (str.length() == 2)) )
	{
		cmd.type = mNULL;
		system("clear");
	}
	else
	{
		cmd.type = mNULL;
		cout << "Invalid Command, Please see help" << endl;
	}

	return cmd;
}

int main(int argc, char* argv[])
{
	signal(SIGINT, disconnect);
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: " << argv[0] << " <host>\n";
			return 1;
		}

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		char port[max_length];
		sprintf(port, "%d", AGENT_PORT);
		tcp::resolver::query query(tcp::v4(), argv[1], port);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		s = new tcp::socket(io_service);
		boost::asio::connect(*s, iterator);


		cout << endl << endl << "HostAP Manager Client v0.2" << endl;
		cout << "Copyright 2015 Kyung Hee University Mobile Convergence Lab" << endl;
		cout << "All rights reserved." << endl;
		cout << "For info, please contact to roy1022@hanamil.net " << endl << endl << endl;
	


		while(1)
		{
			std::cout << std::endl << argv[1] <<  "@ap manager> ";
			char request[max_length];
			Message cmd;
			std::cin.getline(request, max_length);
			cmd = parse_CMD(request);

			if(cmd.type != mNULL)
			{
				size_t cmd_size = sizeof(cmd);
				boost::asio::write(*s, boost::asio::buffer((char *)&cmd, cmd_size));

				Message reply;
				boost::system::error_code error;
				size_t reply_length = boost::asio::read(*s, boost::asio::buffer((char *)&cmd, sizeof(cmd)));

				if(error || (sizeof(cmd) != reply_length))
					throw boost::system::system_error(error);

				if(cmd.type == ACK)
				{
					std::cout << cmd.param;
				}
			}
			bzero(request, max_length);

		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
