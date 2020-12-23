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
		// ��һ���ֽ���ͷ���Ѿ��жϣ�����;
		// end 
		unsigned char* mask = NULL;
		unsigned char* raw_data = NULL;

		if (pkg_len < 2) { // websocket ��ͷû������
			return -1;
		}

		unsigned int len = pkg_data[1];

		// ��ߵ�һ��bitʼ��Ϊ1,����Ҫ����ߵ����bit,��Ϊ0;
		len = (len & 0x0000007f);
		if (len <= 125) { // 4��mask�ֽڣ�ͷ������������
			if (pkg_len < (2 + 4)) { // �޷������� ��С��mask��ֵ
				return -1;
			}
			mask = pkg_data + 2; // ͷ�ֽڣ������ֽ�
		}
		else if (len == 126) { // ���������ֽڱ�ʾ���ȣ�
			if (pkg_len < (4 + 4)) { // 1 + 1 + 2�������ֽ� + 4 MASK
				return -1;
			}
			len = ((pkg_data[2]) | (pkg_data[3] << 8));
			mask = pkg_data + 2 + 2;
		}
		// 1 + 1 + 8(������Ϣ) + 4MASK
		else if (len == 127) { // ����������ÿ���,����ǰ4���ֽڵĴ�С�����治��;
			if (pkg_len < 14) {
				return -1;
			}
			unsigned int low = ((pkg_data[2]) | (pkg_data[3] << 8) | (pkg_data[4] << 16) | (pkg_data[5] << 24));
			unsigned int hight = ((pkg_data[6]) | (pkg_data[7] << 8) | (pkg_data[8] << 16) | (pkg_data[9] << 24));
			if (hight != 0) { // ��ʾ���ĸ��ֽ�������int��Ų��ˣ�̫���ˣ����ǲ�Ҫ�ˡ�
				return -1;
			}
			len = low;
			mask = pkg_data + 2 + 8;
		}
		// mask �̶�4���ֽڣ����Ժ�������ݲ���
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
			body[i] = body[i] ^ mask[i % 4]; // maskֻ��4���ֽڵĳ��ȣ����ԣ�Ҫѭ��ʹ�ã����������ȡ��Ϳ����ˡ�
		}
		//on_json_protocal_recv_entry(body, body_len);
	}

	__inline static void on_json_protocal_recv_entry(unsigned char* data, int len)
	{
		data[len] = 0;
		json_t* root = NULL;
		int ret = json_parse_document(&root, (char*)data);
		if (ret != JSON_OK || root == NULL) { // ����һ��������json��;
			return;
		}

		// ��ȡ�������������ͣ��ٶ�0(key)Ϊ���������ͣ�
		json_t* server_type = json_find_first_label(root, "0");
		server_type = server_type->child;
		if (server_type == NULL || server_type->type != JSON_NUMBER) {
			//assert(0);
			return;
		}
		int stype = atoi(server_type->text); // ��ȡ�����ǵķ���š�
	}
};











#endif // !__CHTTPPARSER_HPP__
