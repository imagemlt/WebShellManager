# WebShellManager
WebShellManager build on cpp with libcurl
## features
* 多个webshell批量管理
* 支持自定义参数位置及加密方式
* 支持需要有附加参数的特殊shell
* 可绕过open_basedir、disable_functions等限制
* 一键不死shell，维持权限
* 附有脚本批量生成可用的shell
* 自定义header、cookie

## build
**CMake**
```bash
cmake .
make
```
**动态编译**
```bash
g++ json.hpp cryptohelper.cpp curlhelper.cpp WebShell.cpp main.cpp -o test -lcurl -lpthread
```
**静态编译**
```bash
g++ json.hpp cryptohelper.cpp curlhelper.cpp WebShell.cpp main.cpp -static -o test -static-libgcc -static-libstdc++ /usr/local/lib/libcurl.a /usr/local/lib/libz.a /usr/local/ssl/lib/libssl.a /usr/local/ssl/lib/libcrypto.a  -ldl -lpthread
```
><del>test文件为x64平台下静态编译好的可执行文件</del>

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
		"cookies":null,
		"headers":null,
		"encrypt":null,
		"place":"COOKIE",
		"placevalue":"xss"
		},
	"pass":"xxx",
	"method":"GET"
	}
```
> note:写为单行,例如：add {"address":"http://baidu.com/shell.php","pass":"mdzz","method":"GET"}  
### 详细解释
  * 格式：json
  * 参数：
    * encrypt(string): 加密方法，目前支持BASE64_ENCODE BASE64_DECODE ROT13 PADDING 格式为"加密方法1 加密参数*加密方法2 加密参数"
    * place(string): 参数位置，值为GET POST COOKIE HEADER 
    * placevalue(string): 与place对应，表示参数
    * addonget(object): 附加的get参数键值对，其中如果键与密码重复<+>将被替换为执行的脚本。
    * addonpost(object): 附加的post参数键值对，与上者相同。
> 所有添加过的shell会以json格式保存于shells.json文件夹下
