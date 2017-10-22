
/**************************************************************************
Copyright:Image
Author: Image
Date:2017-09-05
Description:a curl helper class
***************************************************************************/

#pragma once
#include <iostream> 
#include <string>  
#include<vector>
#include<utility>
#include<map>
#include<algorithm> 
#include<fstream>
#include "curl/curl.h"  

enum METHOD { POST, GET };
enum STATUS { SUCCESS, FAIL };
class curlhelper
{
public:
	static size_t storeFile(void* ptr, size_t size, size_t nmemb, void *filename);
	static size_t storeAns(void *ptr, size_t size, size_t nmemb, void *userdata);
	static std::string UrlDecode(const std::string& szToDecode);
	static std::string UrlEncode(const std::string& szToEncode);
	//static std::string UTF8ToGBK(const std::string& strUTF8);
	//static std::string GBKToUTF8(const std::string& strGBK);
	static CURLcode get(std::string url,
		std::string& ans, 
		std::map<std::string, std::string>*params = NULL,
		std::map<std::string, std::string>*customHeaders = NULL,
		std::map<std::string, std::string>*cookies = NULL
	);
	static CURLcode download_by_post(std::string url, 
		std::string& filename, 
		std::map<std::string, std::string>*params = NULL,
		std::map<std::string, std::string>*customHeaders = NULL,
		std::map<std::string, std::string>*cookies = NULL, 
		std::map<std::string, std::string>*getparams = NULL
	);
	static CURLMcode postfile(std::string url,  
		std::map<std::string, std::string> files,
		std::string& ans,
		std::map<std::string, std::string>*params = NULL, 
		std::map<std::string, std::string>*customHeaders = NULL, 
		std::map<std::string, std::string>*cookies = NULL, 
		std::map<std::string, std::string>*getparams = NULL
	);
	static CURLcode download_by_get(std::string url,
		std::string& filename, 
		std::map<std::string, std::string>*params=NULL, 
		std::map<std::string, std::string>*customHeaders = NULL, 
		std::map<std::string, std::string>*cookies = NULL
	);
	static CURLcode post(std::string url,
		std::string& ans,
		std::map<std::string, std::string>*params = NULL,
		std::map<std::string, std::string>*customHeaders = NULL,
		std::map<std::string, std::string>*cookies = NULL,
		std::map<std::string, std::string>*getparams = NULL,
		bool timeout = true
	);
};

