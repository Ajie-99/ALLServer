#ifndef __HTTP_PARAMETER_H__
#define __HTTP_PARAMETER_H__

class HttpParameter
{
public:
	HttpParameter();

	~HttpParameter(void);

	bool		ParseStringToMap(const std::string& strParam);

	std::string GetResultString();

	bool		HasKey(const std::string& strKey) const;

	INT32		GetIntValue(const std::string& strKey) const;

	std::string GetStrValue(const std::string& strKey) const;

	INT64		GetLongValue(const std::string& strKey) const;

	float		GetFloatValue(const std::string& strKey) const;

	bool		SetKeyValue(const std::string& strKey, INT32 intValue);

	bool		SetKeyValue(const std::string& strKey, std::string& strValue);

	bool		SetKeyValue(const std::string& strKey, INT64 longValue);

	bool		SetKeyValue(const std::string& strKey, float floatValue);

protected:
	std::map<std::string, std::string> m_ParameterMap;
};


#endif /* __HTTP_PARAMETER_H__*/