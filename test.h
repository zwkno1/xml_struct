#ifndef TEST_H
#define TEST_H

namespace test
{

template<class T>
bool xml_get(const rapidxml::xml_node<> * node, T & out)
{
    std::stringstream ss;
    ss << node->value();
    ss >> out;
    return ss.eof();
}

template<>
bool xml_get(const rapidxml::xml_node<> * node, char & out)
{
    if(std::strlen(node->value()) != 1)
        return false;
    out = node->value()[0];
    return true;
}

// qing qiu shebei xingxi
struct ReqDeviceInfo
{
// yong hu id
int    userId;

// she bei id
int    deviceId;

};

bool xml_get(const rapidxml::xml_node<> * , ReqDeviceInfo & );
bool xml_put(rapidxml::xml_node<> * &, const ReqDeviceInfo & );

// fan hui shebei xingxi
struct RetDeviceInfo
{
// yong hu id
int    userId;

// she bei id
int    deviceId;

// she bei zhuang tai
std::vector<ReqDeviceInfo>    deviceInfo;

};

bool xml_get(const rapidxml::xml_node<> * , RetDeviceInfo & );
bool xml_put(rapidxml::xml_node<> * &, const RetDeviceInfo & );

bool xml_get(const rapidxml::xml_node<> * node, ReqDeviceInfo & out)
{
if(!node || std::strcmp(node->name(), "ReqDeviceInfo"))
return false;
node = node->first_node();

if(!node || std::strcmp(node->name(), "userId"))
return false;
if(!xml_get(node, out.userId))
return false;
node = node->next_sibling();

if(!node || std::strcmp(node->name(), "deviceId"))
return false;
if(!xml_get(node, out.deviceId))
return false;
node = node->next_sibling();

return true;
}

bool xml_put(rapidxml::xml_node<> * & node, const ReqDeviceInfo & in)
{
}

bool xml_get(const rapidxml::xml_node<> * node, RetDeviceInfo & out)
{
if(!node || std::strcmp(node->name(), "RetDeviceInfo"))
return false;
node = node->first_node();

if(!node || std::strcmp(node->name(), "userId"))
return false;
if(!xml_get(node, out.userId))
return false;
node = node->next_sibling();

if(!node || std::strcmp(node->name(), "deviceId"))
return false;
if(!xml_get(node, out.deviceId))
return false;
node = node->next_sibling();

if(!node || std::strcmp(node->name(), "Sequence"))
return false;
auto attr = node->first_attribute("name");
if(!attr || std::strcmp(attr->value(), "deviceInfo"))
return false;
for(auto i = node->first_node(); i; i = i->next_sibling())
{
ReqDeviceInfo tmp;
if(!xml_get(i, tmp))
return false;
out.deviceInfo.push_back(tmp);
}
node = node->next_sibling();

return true;
}

bool xml_put(rapidxml::xml_node<> * & node, const RetDeviceInfo & in)
{
}

}
#endif // TEST_H
