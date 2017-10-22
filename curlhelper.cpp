
/**************************************************************************
Copyright:Image
Author: Image
Date:2017-09-05
Description:a curl helper class
***************************************************************************/

#include "curlhelper.h"

/*std::string curlhelper::GBKToUTF8(const std::string& strGBK) {
	std::string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

std::string curlhelper::UTF8ToGBK(const std::string& strUTF8) {
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	WCHAR* wszGBK = new WCHAR[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)strUTF8.c_str(), -1, wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}
*/
std::string curlhelper::UrlEncode(const std::string& szToEncode) {
	std::string src = szToEncode;
	char hex[] = "0123456789ABCDEF";
	std::string dst;
	for (size_t i = 0; i < src.size(); ++i)
	{
		unsigned char cc = src[i];
		if (isascii(cc))
		{
			if (cc == ' ')
			{
				dst += "%20";
			}
			else {

				if (cc == '&')
					dst += "%26";
				else if (cc == '=')
					dst += "%3d";
				else if (cc == '+')
					dst += "%2b";
				else if (cc == ';')
					dst += "%3b";
				else dst += cc;
			}
		}
		else
		{
			unsigned char c = static_cast<unsigned char>(src[i]);
			dst += '%';
			dst += hex[c / 16];
			dst += hex[c % 16];
		}

	}
	//std::cout << dst << std::endl;
	return dst;
}

std::string curlhelper::UrlDecode(const std::string& szToDecode) {
	std::string result;
	int hex = 0;
	for (size_t i = 0; i < szToDecode.length(); ++i)
	{
		switch (szToDecode[i])
		{
		case '+':
			result += ' ';
			break;
		case '%':
			if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
			{
				std::string hexStr = szToDecode.substr(i + 1, 2);
				hex = strtol(hexStr.c_str(), 0, 16);
				//×ÖÄ¸ºÍÊý×Ö[0-9a-zA-Z]¡¢Ò»Ð©ÌØÊâ·ûºÅ[$-_.+!*'(),] ¡¢ÒÔ¼°Ä³Ð©±£Áô×Ö[$&+,/:;=?@] 

				//¿ÉÒÔ²»¾­¹ý±àÂëÖ±½ÓÓÃÓÚURL 
				if (!((hex >= 48 && hex <= 57) || //0-9 
					(hex >= 97 && hex <= 122) ||   //a-z 
					(hex >= 65 && hex <= 90) ||    //A-Z 
												   //Ò»Ð©ÌØÊâ·ûºÅ¼°±£Áô×Ö[$-_.+!*'(),]  [$&+,/:;=?@] 
					hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
					|| hex == 0x2a || hex == 0x2b || hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
					|| hex == 0x3A || hex == 0x3B || hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
					))
				{
					result += char(hex);
					i += 2;
				}
				else result += '%';
			}
			else {
				result += '%';
			}
			break;
		default:
			result += szToDecode[i];
			break;
		}
	}
	return result;
}

size_t curlhelper::storeAns(void *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string *version = (std::string*)userdata;
	version->append((char*)ptr, size * nmemb);
	//*version = UTF8ToGBK(*version);
	//std::cout << *version << std::endl;
	return (size * nmemb);
}

size_t curlhelper::storeFile(void* ptr, size_t size, size_t nmemb, void *filename) {
	std::fstream f(((std::string*)filename)->c_str(), std::ios::app|std::ios::binary);
	f.write((char*)ptr, size*nmemb);
	f.close();
	return (size*nmemb);
}

CURLcode curlhelper::get(std::string url,//url
	std::string& ans, //store the ans
	std::map<std::string, std::string>*params,//get params 
	std::map<std::string, std::string>*customHeaders,//some extra headers
	std::map<std::string, std::string>*cookies //some extra cookies
)
{
	CURL* curl;
	CURLcode res;
	char buffer[10] = { 0 };
	struct curl_slist *headers = NULL;
	curl = curl_easy_init();//curl³õÊ¼»¯  
	if (params != NULL) {
		if (url.find("?") == std::string::npos)url = url + '?';
		else url = url + '&';
		for (std::map<std::string, std::string>::iterator it = params->begin(); it != params->end(); it++) {
			url = url + it->first + '=' + it->second + '&';
		}
	}
	if (curl)
	{
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Connections")) == customHeaders->end())
			headers = curl_slist_append(headers, "Connection:keep-alive");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept")) == customHeaders->end())
			headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,* / *;q=0.8");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Content-type")) == customHeaders->end())
			headers = curl_slist_append(headers, "Content-type:application/x-www-form-urlencoded");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept-Language")) == customHeaders->end())
			headers = curl_slist_append(headers, "Accept-Language:zh-CN,zh;q=0.8");
		if (customHeaders != NULL) {
			for (std::map<std::string, std::string>::iterator it = customHeaders->begin(); it != customHeaders->end(); it++) {
				headers = curl_slist_append(headers, (it->first + ": " + it->second).c_str());
			}
		}
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); 
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); 
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt"); 
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/cookies.txt"); 
		if (cookies != NULL) {
			for (std::map<std::string, std::string>::iterator it = cookies->begin(); it != cookies->end(); it++) {
				curl_easy_setopt(curl, CURLOPT_COOKIE, (it->first + "=" + it->second).c_str());
			}
		}
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storeAns); 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ans);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			ans = curl_easy_strerror(res);
		}
		curl_easy_cleanup(curl);
	}
	else {
		exit(0);
	}
	return res;
}

CURLcode curlhelper::download_by_get(std::string url, 
	std::string&filename,
	std::map<std::string, std::string>*params, 
	std::map<std::string, std::string>*customHeaders , 
	std::map<std::string, std::string>*cookies
) {
	CURL* curl;
	CURLcode res;
	char buffer[10] = { 0 };
	struct curl_slist *headers = NULL;
	curl = curl_easy_init();//curl³õÊ¼»¯  
	if (params != NULL) {
		if (url.find("?") == std::string::npos)url = url + '?';
		else url += '&';
		for (std::map<std::string, std::string>::iterator it = params->begin(); it != params->end(); it++) {
			url = url + it->first + '=' + it->second + '&';
		}
	}
	if (curl)
	{
		if(customHeaders==NULL || customHeaders!=NULL && customHeaders->find(std::string("Connections"))==customHeaders->end())
			headers = curl_slist_append(headers, "Connection:keep-alive");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept")) == customHeaders->end())
			headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,* / *;q=0.8");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Content-type")) == customHeaders->end())
			headers = curl_slist_append(headers, "Content-type:application/x-www-form-urlencoded");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept-Language")) == customHeaders->end())
			headers = curl_slist_append(headers, "Accept-Language:zh-CN,zh;q=0.8");
		if (customHeaders != NULL) {
			for (std::map<std::string, std::string>::iterator it = customHeaders->begin(); it != customHeaders->end(); it++) {
				headers = curl_slist_append(headers, (it->first + ": " + it->second).c_str());
			}
		}
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); 
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate"); 
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/cookies.txt"); 
		if (cookies != NULL) {
			for (std::map<std::string, std::string>::iterator it = cookies->begin(); it != cookies->end(); it++) {
				curl_easy_setopt(curl, CURLOPT_COOKIE, (it->first + "=" + it->second).c_str());
			}
		}
		curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT, 20);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,storeFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &filename);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	else {
		exit(0);
	}
	return res;
}

CURLcode curlhelper::post(std::string url,
	std::string& ans, 
	std::map<std::string, std::string>*params, 
	std::map<std::string, std::string>*customHeaders,
	std::map<std::string, std::string>*cookies,
	std::map<std::string,std::string>*getparams, //extra params in the url
	bool timeout
) {
	CURL* curl;
	CURLcode res;
	char buffer[10] = { 0 };
	struct curl_slist *headers = NULL;
	curl = curl_easy_init();//curl³õÊ¼»¯  
	if (getparams != NULL) {
		if (url.find("?") == std::string::npos)url = url + '?';
		else url += '&';
		for (std::map<std::string, std::string>::iterator it = getparams->begin(); it != getparams->end(); it++) {
			url = url + it->first + '=' + it->second + '&';
		}
	}
	if (curl)
	{
		std::string postfileds;
		curl_easy_setopt(curl, CURLOPT_POST, true);
		if (params != NULL) {
			//std::cout << "mdzz" << std::endl;
			for (std::map<std::string, std::string>::iterator it = params->begin(); it != params->end(); it++) {
				postfileds += it->first + '=' + it->second + '&';
				
			}
			//std::cout << postfileds << std::endl;
		}
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfileds.c_str());
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Connections")) == customHeaders->end())
			headers = curl_slist_append(headers, "Connection:keep-alive");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept")) == customHeaders->end())
			headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,* / *;q=0.8");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Content-type")) == customHeaders->end())
			headers = curl_slist_append(headers, "Content-type:application/x-www-form-urlencoded");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept-Language")) == customHeaders->end())
			headers = curl_slist_append(headers, "Accept-Language:zh-CN,zh;q=0.8");
		if (customHeaders != NULL) {
			for (std::map<std::string, std::string>::iterator it = customHeaders->begin(); it != customHeaders->end(); it++) {
				headers = curl_slist_append(headers, (it->first + ": " + it->second).c_str());
			}
		}
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/cookies.txt");
		if (cookies != NULL) {
			for (std::map<std::string, std::string>::iterator it = cookies->begin(); it != cookies->end(); it++) {
				curl_easy_setopt(curl, CURLOPT_COOKIE, (it->first + "=" + it->second).c_str());
			}
		}
		if (timeout) {
			curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 10);
			curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 5);
		}
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storeAns);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ans);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			ans = curl_easy_strerror(res);
		}
		curl_easy_cleanup(curl);
	}
	else {
		exit(0);
	}
	//std::cout<<ans;
	return res;
}

CURLMcode curlhelper::postfile(std::string url,
	std::map<std::string,std::string>files,
	std::string& ans, 
	std::map<std::string, std::string>*params, 
	std::map<std::string, std::string>*customHeaders, 
	std::map<std::string, std::string>*cookies,
	std::map<std::string,std::string>*getparams
) {
	CURL *curl;
	CURLMcode res;
	CURLM *multi_handle;
	int still_running;
	//struct curl_slist *headers = NULL;
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	if (getparams != NULL) {
		if (url.find("?") == std::string::npos)url = url + '?';
		else url += '&';
		for (std::map<std::string, std::string>::iterator it = getparams->begin(); it != getparams->end(); it++) {
			url = url + it->first + '=' + it->second + '&';
		}
	}
	/* Fill in the file upload field. This makes libcurl load data from
	the given file name when curl_easy_perform() is called. */
	for (std::map<std::string, std::string>::iterator it = files.begin(); it != files.end(); it++) {
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, it->first.c_str(),
			CURLFORM_FILE, it->second.c_str(),
			CURLFORM_END);

		/* Fill in the filename field */
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "filename",
			CURLFORM_COPYCONTENTS, it->second.c_str(),
			CURLFORM_END);
	}
	if (params != NULL) {
		for (std::map<std::string, std::string>::iterator it = params->begin(); it != params->end(); it++) {
			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, it->first.c_str(),
				CURLFORM_COPYCONTENTS, it->second.c_str(),
				CURLFORM_END);
		}
	}
	curl = curl_easy_init();
	multi_handle = curl_multi_init();
	if (customHeaders != NULL) {
		for (std::map<std::string, std::string>::iterator it = customHeaders->begin(); it != customHeaders->end(); it++) {
			headerlist = curl_slist_append(headerlist, (it->first + ":" + it->second).c_str());
		}
	}
	headerlist = curl_slist_append(headerlist, buf);
	if (curl && multi_handle) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/cookies.txt");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storeAns);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ans);
		if (cookies != NULL) {
			for (std::map<std::string, std::string>::iterator it = cookies->begin(); it != cookies->end(); it++) {
				curl_easy_setopt(curl, CURLOPT_COOKIE, (it->first + "=" + it->second).c_str());
			}
		}
		curl_multi_add_handle(multi_handle, curl);

		res=curl_multi_perform(multi_handle, &still_running);

		do {
			struct timeval timeout;
			int rc; /* select() return code */

			fd_set fdread;
			fd_set fdwrite;
			fd_set fdexcep;
			int maxfd = -1;

			long curl_timeo = -1;

			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdexcep);
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			curl_multi_timeout(multi_handle, &curl_timeo);
			if (curl_timeo >= 0) {
				timeout.tv_sec = curl_timeo / 1000;
				if (timeout.tv_sec > 1)
					timeout.tv_sec = 1;
				else
					timeout.tv_usec = (curl_timeo % 1000) * 1000;
			}
			curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
			rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);

			switch (rc) {
			case -1:
				break;
			case 0:
			default:
				curl_multi_perform(multi_handle, &still_running);
				break;
			}
		} while (still_running);
		curl_multi_cleanup(multi_handle);
		curl_easy_cleanup(curl);
		curl_formfree(formpost);
		curl_slist_free_all(headerlist);
	}
	return res;
}

CURLcode curlhelper::download_by_post(std::string url, 
	std::string&filename, 
	std::map<std::string, std::string>*params, 
	std::map<std::string, std::string>*customHeaders , 
	std::map<std::string, std::string>*cookies , 
	std::map<std::string, std::string>*getparams 
) {
	CURL* curl;
	CURLcode res;
	char buffer[10] = { 0 };
	struct curl_slist *headers = NULL;
	if (getparams != NULL) {
		if (url.find("?") == std::string::npos)url = url + '?';
		else url += '&';
		for (std::map<std::string, std::string>::iterator it = getparams->begin(); it != getparams->end(); it++) {
			url = url + it->first + '=' + it->second + '&';
		}
	}
	curl = curl_easy_init();//curl³õÊ¼»¯  
							//if (url.find("?") != std::string::npos)url = url + '?';
	
	if (curl)
	{
		std::string postfileds;
		curl_easy_setopt(curl, CURLOPT_POST, true);
		if (params != NULL) {
			for (std::map<std::string, std::string>::iterator it = params->begin(); it != params->end(); it++) {
				postfileds = postfileds + it->first + '=' + it->second + '&';
			}
		}
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfileds.c_str());
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Connections")) != customHeaders->end())
			headers = curl_slist_append(headers, "Connection:keep-alive");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept")) != customHeaders->end())
			headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,* / *;q=0.8");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Content-type")) != customHeaders->end())
			headers = curl_slist_append(headers, "Content-type:application/x-www-form-urlencoded");
		if (customHeaders == NULL || customHeaders != NULL && customHeaders->find(std::string("Accept-Language")) != customHeaders->end())
			headers = curl_slist_append(headers, "Accept-Language:zh-CN,zh;q=0.8");
		if (customHeaders != NULL) {
			for (std::map<std::string, std::string>::iterator it = customHeaders->begin(); it != customHeaders->end(); it++) {
				headers = curl_slist_append(headers, (it->first + ": " + it->second).c_str());
			}
		}
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookies.txt");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/cookies.txt");
		if (cookies != NULL) {
			for (std::map<std::string, std::string>::iterator it = cookies->begin(); it != cookies->end(); it++) {
				curl_easy_setopt(curl, CURLOPT_COOKIE, (it->first + "=" + it->second).c_str());
			}
		}
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storeFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &filename);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	else {
		exit(0);
	}
	return res;
}
