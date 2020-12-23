#pragma once
#ifndef __CHTTPPARSER_HPP__
#define __CHTTPPARSER_HPP__
extern "C"
{
#include "../http_parser/http_parser.h"
#include "../crypt/sha1.h"
#include "../crypt/base64_encoder.h"
#include "../mjson/json.h"
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
	__inline static int json_parse_document(json_t** root, const char* text)
	{
		enum json_error error;
		struct json_parsing_info* jpi;

		assert(root != NULL);
		assert(*root == NULL);
		assert(text != NULL);

		/* initialize the parsing structure */
		jpi = (struct json_parsing_info*)malloc(sizeof(struct json_parsing_info));
		if (jpi == NULL)
		{
			return JSON_MEMORY;
		}
		json_jpi_init(jpi);
		error = json_parse_fragment(jpi, text);
		if ((error == JSON_WAITING_FOR_EOF) || (error == JSON_OK))
		{
			*root = jpi->cursor;
			free(jpi);
			return JSON_OK;
		}
		else
		{
			free(jpi);
			return error;
		}
	}
	__inline static int recv_ws_header(unsigned char* pkg_data, int pkg_len,int* pkg_size, int* out_header_size)
	{
		// 第一个字节是头，已经判断，跳过;
		// end 
		unsigned char* mask = NULL;
		unsigned char* raw_data = NULL;

		if (pkg_len < 2) { // websocket 包头没有收完
			return -1;
		}

		unsigned int len = pkg_data[1];

		// 最高的一个bit始终为1,我们要把最高的这个bit,变为0;
		len = (len & 0x0000007f);
		if (len <= 125) { // 4个mask字节，头才算收完整；
			if (pkg_len < (2 + 4)) { // 无法解析出 大小与mask的值
				return -1;
			}
			mask = pkg_data + 2; // 头字节，长度字节
		}
		else if (len == 126) { // 后面两个字节表示长度；
			if (pkg_len < (4 + 4)) { // 1 + 1 + 2个长度字节 + 4 MASK
				return -1;
			}
			len = ((pkg_data[2]) | (pkg_data[3] << 8));
			mask = pkg_data + 2 + 2;
		}
		// 1 + 1 + 8(长度信息) + 4MASK
		else if (len == 127) { // 这种情况不用考虑,考虑前4个字节的大小，后面不管;
			if (pkg_len < 14) {
				return -1;
			}
			unsigned int low = ((pkg_data[2]) | (pkg_data[3] << 8) | (pkg_data[4] << 16) | (pkg_data[5] << 24));
			unsigned int hight = ((pkg_data[6]) | (pkg_data[7] << 8) | (pkg_data[8] << 16) | (pkg_data[9] << 24));
			if (hight != 0) { // 表示后四个字节有数据int存放不了，太大了，我们不要了。
				return -1;
			}
			len = low;
			mask = pkg_data + 2 + 8;
		}
		// mask 固定4个字节，所以后面的数据部分
		raw_data = mask + 4;
		*out_header_size = (raw_data - pkg_data);
		*pkg_size = len + (*out_header_size);
		// printf("data length = %d\n", len);
		return 0;
	}

	__inline static void parser_ws_pack(unsigned char* body, int body_len, unsigned char* mask)
	{
		for (int i = 0; i < body_len; i++)
		{
			body[i] = body[i] ^ mask[i % 4]; // mask只有4个字节的长度，所以，要循环使用，如果超出，取余就可以了。
		}
		//on_json_protocal_recv_entry(body, body_len);
	}

	__inline static void on_json_protocal_recv_entry(unsigned char* data, int len)
	{
		data[len] = 0;
		json_t* root = NULL;
		int ret = json_parse_document(&root, (char*)data);
		if (ret != JSON_OK || root == NULL) { // 不是一个正常的json包;
			return;
		}

		// 获取这个命令服务类型，假定0(key)为服务器类型；
		json_t* server_type = json_find_first_label(root, "0");
		server_type = server_type->child;
		if (server_type == NULL || server_type->type != JSON_NUMBER) {
			//assert(0);
			return;
		}
		int stype = atoi(server_type->text); // 获取了我们的服务号。
	}
};











#endif // !__CHTTPPARSER_HPP__
