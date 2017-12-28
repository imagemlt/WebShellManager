# WebShellManager
WebShellManager build on cpp with libcurl
## build
**动态编译**
```bash
g++ json.hpp cryptohelper.cpp curlhelper.cpp WebShell.cpp main.cpp -o test -lcurl -lpthread
```
**静态编译**
```bash
g++ json.hpp cryptohelper.cpp curlhelper.cpp WebShell.cpp main.cpp -static -o test -static-libgcc -static-libstdc++ /usr/local/lib/libcurl.a /usr/local/lib/libz.a /usr/local/ssl/lib/libssl.a /usr/local/ssl/lib/libcrypto.a  -ldl -lpthread
```
>test文件为x64平台下静态编译好的可执行文件

>note:由于使用[https://github.com/nlohmann/json](https://github.com/nlohmann/json)因此需编译器支持C++11
## usage
* list :列出所有的shell
* delete index:删除指定index的shell
* add json:添加shell，格式为json
* execute index 命令:执行命令，当index为all时在所有shell中执行
* push index 本地文件路径 目标文件路径:上传本地文件到制定路径
* neverdie index:一键不死shell

## json format:
```json
	{
	"address":"地址",
	"custom":{
		"addonget":null,
		"addonpost":null,
		"encrypt":null,
		"place":"COOKIE",
		"placevalue":"xss"
		},
	"pass":"xxx",
	"method":"GET"
	}
```
> note:写为单行,例如：add {"address":"http://baidu.com/shell.php","pass":"mdzz","method":"GET"}  
具体格式参考[这里](https://github.com/imagemlt/EasyKnife)
