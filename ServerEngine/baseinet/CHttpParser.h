#pragma once
#ifndef __CHTTPPARSER_HPP__
#define __CHTTPPARSER_HPP__
extern "C"
{
#include "../http_parser/http_parser.h"
#include "../crypt/sha1.h"
#include "../crypt/base64_encoder.h"
}


class CHttpParser
{
public:
	static char	header_key[64];
	static char	client_ws_key[128];
	static int		has_client_key;



	__inline static int on_header_field(http_parser* p, const char* at, size_t length)
	{
		length = (length < 63) ? length : 63;
		strncpy(header_key, at, length);

		header_key[length] = 0;
		//std::cout << "header_key == " << header_key << std::endl;
		return 0;
	}
	__inline static int on_header_value(http_parser* p, const char* at, size_t length)
	{
		if (strcmp(header_key, "Sec-WebSocket-Key") != 0)
		{
			return 0;
		}
		length = (length < 127) ? length : 127;

		strncpy(client_ws_key, at, length);
		client_ws_key[length] = 0;
		//std::cout << "client_ws_key == " << client_ws_key << std::endl;
		has_client_key = 1;

		return 0;
	}


};











#endif // !__CHTTPPARSER_HPP__
