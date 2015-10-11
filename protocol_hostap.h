#ifndef __PT_H__
#define __PT_H__
/********************
 @file protocol_hostap.h
 @date 2015/10/11
 @author 안계완
 @brief
 	declare message types, and ap config structure
*********************/


#define AGENT_PORT 12013

#include <iostream>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

using namespace std;

typedef enum Msg_types{
	mNULL = 0,
	START_AP = 1,
	STOP_AP,
	GET_STATUS,
	CHANGE_SSID,
	CHANGE_PWD,
	OFF_PWD,
	CHANGE_TX,
	CHANGE_CHANNEL,
	CHANGE_MODE,
	CHANGE_HIDE,
	BUILD_CONFIGURE,
	DISCONNECT,
	ACK
} msg_types;

typedef struct
{
	msg_types type;
	char param[256];
} Message;

typedef struct {
	string interface;
	bool isBridge;
	string bridge;
	string ssid;
	string channel;
	string hw_mode;
	bool isPwd;
	string pwd;
	bool isHide;
	string hide;
} ap_config;


#endif
