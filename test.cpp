#include<iostream>
#include<string>
#include<vector>
#include<typeinfo>
#include"json.hpp"
using namespace std;
using json = nlohmann::json;

int main(){
	json j_patch = R"([
  { "op": "replace", "path": "/baz", "value": "boo" },
  { "op": "add", "path": "/hello", "value": ["world"] },
  { "op": "remove", "path": "/foo"}
])"_json;
		for(json::iterator it=j_patch.begin();it!=j_patch.end();++it){
			  						for(json::iterator getIter=(*it).begin();getIter!=(*it).end();++getIter){
												cout<<typeid(getIter.key()).name()<<endl;
												string s="mdzz";
												cout<<typeid(s).name()<<endl;
																								}
		}
}
