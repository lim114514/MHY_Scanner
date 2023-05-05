﻿#include "LoginBili.h"
void LoginBili::openConfig()
{
    inFile.open("./Config/config_private.json");
    std::stringstream configStringStream;
    configStringStream << inFile.rdbuf();
    const std::string& configString = configStringStream.str();
    configJson.parse(configString);
}

void LoginBili::updateConfig()//先放这里
{
    const std::string output = configJson.str();
    std::ofstream outFile("./Config/config_private.json");
    std::stringstream outStr;
    bool isInPair = false;
    for (int i = 0; i < output.size(); i++)
    {
        if (output[i] == '{')
        {
            outStr << "{\n";
            continue;
        }
        if (output[i] == '}')
        {
            outStr << "\n}";
            isInPair = false;
            continue;
        }
        if (output[i] == ',')
        {
            outStr << ",\n";
            isInPair = false;
            continue;
        }
        if (!isInPair)
        {
            outStr << "  ";
            isInPair = true;
        }
        outStr << output[i];
    }
    outFile << outStr.str();
    outFile.close();
}

void LoginBili::setAutoStart(bool state)
{
    if (state)
    {
        configJson["is_auto_start"] = true;
    }
    else
    {
        configJson["is_auto_start"] = false;
    }
    updateConfig();
}

bool LoginBili::getAutoStart()
{
    return (bool)configJson["is_auto_start"];
}

void LoginBili::setAutoExit(bool Exit)
{
    
    if (Exit)
    {
        configJson["is_auto_exit"] = true;
    }
    else
    {
        configJson["is_auto_exit"] = false;
    }
    updateConfig();
}

bool LoginBili::getAutoExit()
{
    return (bool)configJson["is_auto_exit"];;
}

//检查access_key和uid是否有效
int LoginBili::loginBiliKey(std::string& realName)
{
    uid = configJson["uid"];
    access_key = configJson["access_key"];
    json::Json userInfo = getUserInfo(uid, access_key);
    int code = (int)userInfo["code"];
    if (code != 0)
    {
        return code;
    }
    realName = HttpClient::string_To_UTF8(userInfo["uname"]);
    return 0;
}

int LoginBili::loginBiliPwd(std::string Account, std::string Pwd, std::string& message)
{
    std::string a = login1(Account, Pwd);
    a = HttpClient::UTF8_To_string(a);
    json::Json loginJ;
    loginJ.parse(a);
    int code = (int)loginJ["code"];
    if (code != 0)
    {
        message = loginJ["message"];
        return code;
    }
    configJson["account"] = Account;
    configJson["password"] = Pwd;
    uid = loginJ["uid"];
    access_key = loginJ["access_key"];
    configJson["uid"] = uid;
    configJson["access_key"] = access_key;
    loginJ.clear();
    configJson["signed_in"] = true;
    userInfo = getUserInfo(uid, access_key);
    configJson["realname"] = HttpClient::string_To_UTF8(userInfo["uname"]);
    updateConfig();
    return code;
}