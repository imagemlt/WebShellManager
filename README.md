# WebShellManager
WebShellManager build on cpp with libcurl
## build
```bash
g++ cryptohelper.cpp curlhelper.cpp WebShell.cpp main.cpp -o test -lcurl
```

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
[参考](https://github.com/imagemlt/EasyKnife)
