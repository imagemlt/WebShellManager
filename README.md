# WebShellManager
WebShellManager build on cpp with libcurl
## build
```bash
g++ json.hpp cryptohelper.cpp curlhelper.cpp WebShell.cpp main.cpp -o test -lcurl
```
>note:由于使用[https://github.com/nlohmann/json](https://github.com/nlohmann/json)因此需编译器支持C++11
## usage
* list :列出所有的shell
* delete index:删除指定index的shell
* add json:添加shell，格式为json
* execute index 命令:执行命令，当index为all时在所有shell中执行

## json format:
```json
	{
	"address":"地址",
	"custom":{
		"addonget":null,
		"encrypt":null,
		"place":"COOKIE",
		"placevalue":"xss"
		},
	"pass":"xxx",
	"method":"GET"
	}
```
> note:写为单行,例如：add {"address":"http://baidu.com/shell.php","pass":"mdzz","method":"GET"}
[参考](https://github.com/imagemlt/EasyKnife)
