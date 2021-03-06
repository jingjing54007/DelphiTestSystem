import json
import os.path
from shutil import copyfile

#添加环境变量
os.environ["path"] += ";C:/Qt/Qt5.9.1/5.9.1/mingw53_32/bin;C:/Qt/Qt5.9.1/Tools/mingw530_32/bin;"
#生成MAKEFILE
os.system("qmake.exe -o Makefile ..\..\WKELightWave\WKELightWave.pro -spec win32-g++ ")
os.system("mingw32-make -f Makefile.Release clean")
#编译程序
os.system("mingw32-make -j -f Makefile.Release")
#执行文件
#os.system("..\Libs\WKELightWave.exe")

#读取Version.txt 文件 获取编译后的版本号
version_file = open('../Images/version.txt', 'r', encoding='utf-8')

i=0
change_log = ""
for lines in version_file:
    if lines.strip() !=""  and i==0:
        version_str = lines
        i += 1
    elif lines.strip() !="" and i>0 :
        change_log += lines
        i += 1
    elif lines.strip() == "":
        break
    else:
        pass
version_file.close()

print(version_str)
print(change_log)
print("End")

jsonContent = {
    "updates":{
        "windows":{
            "open-url":"",
            "download-url":"https://raw.githubusercontent.com/Leroy888/DelphiTestSystem/abama/Installer/WKELightWave.exe",
            "defs-url":"https://raw.githubusercontent.com/Leroy888/DelphiTestSystem/abama/Installer/updates.json", #本Json文件放的网络地址
            "latest-version":version_str,
            "changelog": change_log
        }
    }
}

str_content =json.dumps(jsonContent, sort_keys=True, indent=4, separators=(",", ':'))
print(str_content)

print("删除了WKELightWave.exe文件")
os.chdir("../Setup")
os.system("ISCC.exe mySetupV1.1.iss")

print("生成了WKELightWave.exe文件")

#写入新的Json文件
os.chdir("../Installer")
update_json = open("./updates.json", 'w')
update_json.write(str_content)
update_json.close()

#讲update.json 拷贝到Run目录里面
copyfile("./updates.json", "../Run/update.json")


print(r"写入新的Json文件")

os.system("git status")
os.system("git add -A")
os.system('git commit -m "%s"' % (change_log))
os.system("git push origin abama")

