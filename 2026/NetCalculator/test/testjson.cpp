#include <jsoncpp/json/json.h>
#include <iostream>
#include <string>
int main()
{
    // 序列化
    Json::Value root;
    root["name"] = "zhangsan";
    root["high"] = 1.78f;
    root["sex"] = "nan";

    // std::string s = root.toStyledString();
    // std::cout << s << std::endl;

    // Json::FastWriter writer;
    Json::StyledWriter writer;
    std::string s = writer.write(root);
    std::cout << s << std::endl;

    // 反序列化
    Json::Value droot;
    Json::Reader reader;
    reader.parse(s, droot);

    std::string name = droot["name"].asString();
    float hight = droot["high"].asFloat();
    std::string sex = droot["sex"].asString();
    std::cout << name << hight << sex << std::endl;
    return 0;
}