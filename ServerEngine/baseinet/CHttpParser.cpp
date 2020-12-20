#include <iostream>
#include "CHttpParser.h"




char	CHttpParser::header_key[64] = {0};
char	CHttpParser::client_ws_key[128] = {0};
int		CHttpParser::has_client_key = 0;