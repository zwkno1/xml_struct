#include "xmlstruct.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <rapidxml.h>
#include <rapidxml_print.h>
#include <rapidxml_utils.h>
#include <cstring>

std::string xml = "<RetDeviceInfo>"
                        "<userId>123</userId>"
                        "<deviceId>123243</deviceId>"
                        "<Sequence name='deviceInfo'>"
                            "<ReqDeviceInfo>"
                                "<userId>7234</userId>"
                                "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                            "<ReqDeviceInfo>"
                                "<userId>6234</userId>"
                                "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                            "<ReqDeviceInfo>"
                                "<userId>5234</userId>"
                                "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                            "<ReqDeviceInfo>"
                                "<userId>4234</userId>"
                                "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                        "</Sequence>"
                  "</ReqDeviceInfo>";

struct ReqDeviceInfo
{
    int    userId;
    int    deviceId;
};

struct RetDeviceInfo
{
    int    userId;
    int    deviceId;
    std::vector<ReqDeviceInfo>    deviceInfo;
};

template<typename Dst, typename Src>
Dst getValue(Src src)
{
    Dst ret;
    std::stringstream ss;
    ss << src;
    ss >> ret;
    if(!ss.eof())
        throw std::string("cast err");
    return ret;
}

void xmlI()
{
    //a sample of get struct from xml
    ReqDeviceInfo ret;
    rapidxml::xml_document<> doc;
    doc.parse<0>((char *)xml.data());
    auto node = doc.first_node();
    std::cout << node->value() <<std::endl;
    if(!node || std::strcmp(node->name(), "RetDeviceInfo"))
        throw std::string("unexpect node: ") + node->name();
    node = node->first_node();

    if(!node || std::strcmp(node->name(), "userId"))
        throw std::string("unexpect node: ") + node->name();
    ret.userId = getValue<int>(node->value());
    node = node->next_sibling();

    if(!node || std::strcmp(node->name(), "deviceId"))
        throw std::string("unexpect node: ") + node->name();
    ret.deviceId = getValue<int>(node->value());
    node = node->next_sibling();

    if(!node || std::strcmp(node->name(), "Sequence"))
        throw std::string("unexpect node: ") + node->name();
    auto attr = node->first_attribute("name");
    if(!attr || std::strcmp(attr->value(), "deviceInfo"))
        throw std::string("unexpect node: ") + node->name();
    for(auto i = node->first_node(); i; i = i->next_sibling())
    {
        char a;
        a = getValue<char>(node->value());
    }
    node = node->next_sibling();
}
/*
struct ReqDeviceInfo
{
    int    userId;
    int    deviceId;
};

struct RetDeviceInfo
{
    int    userId;
    int    deviceId;
    std::vector<ReqDeviceInfo>    deviceInfo;
};
*/
template<class T>
std::string getStr(const T & in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

void xmlO()
{
    RetDeviceInfo in;
    in.userId = 1000;
    in.deviceId = 2000;
    for(int i = 0; i < 10; ++i)
    {
        ReqDeviceInfo tmp; tmp.userId = i * 1000; tmp.deviceId = (i+1)*1000;
        in.deviceInfo.push_back(tmp);
    }

    rapidxml::xml_document<> doc;

    //struct
    auto node = doc.allocate_node(rapidxml::node_element, doc.allocate_string("RetDeviceInfo"));
    doc.append_node(node);

    //item
    auto node1 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("userId"), doc.allocate_string(getStr(in.userId).c_str()));
    node->append_node(node1);

    //item
    auto node2 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("deviceId"), doc.allocate_string(getStr(in.deviceId).c_str()));
    node->append_node(node2);

    //seq
    auto node3 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("Sequence"));
    node->append_node(node3);
    {
        for(auto & i : in.deviceInfo)
        {
            //struct
            auto node4 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("ReqDeviceInfo"));
            node3->append_node(node4);

            //item
            auto node5 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("userId"), doc.allocate_string(getStr(i.userId).c_str()));
            node4->append_node(node5);

            //item
            auto node6 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("deviceId"), doc.allocate_string(getStr(i.deviceId).c_str()));
            node4->append_node(node6);
        }
    }

    std::string str;
    rapidxml::print(std::back_inserter(str),doc,0);
    std::cout << str << std::endl;
    std::cout << std::endl;


}


// for test
//#define TEST
#ifdef TEST
#include "test.h"
void testI()
{

    rapidxml::xml_document<> doc;
    doc.parse<0>((char *)xml.data());
    auto node = doc.first_node();
    test::RetDeviceInfo info;
    if(test::xml_get(node, info))
        std::cout << "ok" << std::endl;

}
#endif



int main(int argc, char *argv[])
{
#ifdef TEST
    testI();
    xmlO();
#endif
    std::string xml("<struct name='ReqDeviceInfo' type='struct' def='struct' desc='qing qiu shebei xingxi'> "
                    "<item name='userId' type='base' def='int' desc='yong hu id'></item>"
                    "<item name='deviceId' type='base' def='int' desc='she bei id'></item>"
                    "</struct>"
                    "<struct name='RetDeviceInfo' type='struct' def='struct' desc='fan hui shebei xingxi'> "
                    "<item name='userId' type='base' def='int' desc='yong hu id'></item>"
                    "<item name='deviceId' type='base' def='int' desc='she bei id'></item>"
                    //"<item name='deviceInfo' type='seq' def='std::vector<ReqDeviceInfo>' base_type='ReqDeviceInfo' desc='she bei zhuang tai'></item>"
                    "<item name='deviceInfo' type='struct' def='ReqDeviceInfo' desc='she bei zhuang tai'></item>"
                    "</struct>"
                    );
    std::stringstream ss;
    try{
        xmlstruct::parser p(xml);
        p.genDef(ss);
        p.genCodeI(ss);
        p.genCodeO(ss);
        std::cout << ss.str();
    }
    catch(const std::string & str)
    {
        std::cout << str << std::endl;
        return -1;
    }

    return 0;
}
