#ifndef TEST_H
#define TEST_H

namespace test
{

template<class T>
bool xml_struct(const rapidxml::xml_node<> * node, T & out)
{
    std::stringstream ss;
    ss << node->value();
    ss >> out;
    return ss.eof();
}

template<>
bool xml_struct(const rapidxml::xml_node<> * node, char & out)
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

bool xml_struct(const rapidxml::xml_node<> * , ReqDeviceInfo & );
bool struct_xml(rapidxml::xml_node<> * &, const ReqDeviceInfo & );

// fan hui shebei xingxi
struct RetDeviceInfo
{
// yong hu id
int    userId;

// she bei id
int    deviceId;

// she bei zhuang tai
::std::vector< ReqDeviceInfo >    deviceInfos;

// she bei zhuang tai
::std::vector< int >    deviceInfos2;

// she bei zhuang tai
ReqDeviceInfo    deviceInfo;

// she bei shi jian
int    time;

};

bool xml_struct(const rapidxml::xml_node<> * , RetDeviceInfo & );
bool struct_xml(rapidxml::xml_node<> * &, const RetDeviceInfo & );

bool xml_struct(const rapidxml::xml_node<> * node, ReqDeviceInfo & out)
{
if(!node || std::strcmp(node->name(), "ReqDeviceInfo"))
return false;
node = node->first_node();

/* @generate [ userId:int ] begin @ */
if(!node || std::strcmp(node->name(), "userId"))
return false;
if(!xml_struct(node, out.userId))
return false;
node = node->next_sibling();
/* @generate [ userId:int ] end @ */

/* @generate [ deviceId:int ] begin @ */
if(!node || std::strcmp(node->name(), "deviceId"))
return false;
if(!xml_struct(node, out.deviceId))
return false;
node = node->next_sibling();
/* @generate [ deviceId:int ] end @ */

return true;
}

bool xml_struct(const rapidxml::xml_node<> * node, RetDeviceInfo & out)
{
if(!node || std::strcmp(node->name(), "RetDeviceInfo"))
return false;
node = node->first_node();

/* @generate [ userId:int ] begin @ */
if(!node || std::strcmp(node->name(), "userId"))
return false;
if(!xml_struct(node, out.userId))
return false;
node = node->next_sibling();
/* @generate [ userId:int ] end @ */

/* @generate [ deviceId:int ] begin @ */
if(!node || std::strcmp(node->name(), "deviceId"))
return false;
if(!xml_struct(node, out.deviceId))
return false;
node = node->next_sibling();
/* @generate [ deviceId:int ] end @ */

/* @generate [ deviceInfos:std::vector< ReqDeviceInfo > ] begin @ */
if(!node || std::strcmp(node->name(), "deviceInfos"))
return false;
{
auto tmp = node->first_node();
if(!tmp || std::strcmp(tmp->name(), "Vector"))
return false;
for(auto i = tmp->first_node(); i; i = i->next_sibling())
{
ReqDeviceInfo tmp;
if(!xml_struct(i, tmp))
return false;
out.deviceInfos.push_back(tmp);
}
}
node = node->next_sibling();
/* @generate [ deviceInfos:std::vector< ReqDeviceInfo > ] end @ */

/* @generate [ deviceInfos2:std::vector< int > ] begin @ */
if(!node || std::strcmp(node->name(), "deviceInfos2"))
return false;
{
auto tmp = node->first_node();
if(!tmp || std::strcmp(tmp->name(), "Vector"))
return false;
for(auto i = tmp->first_node(); i; i = i->next_sibling())
{
int tmp;
if(!xml_struct(i, tmp))
return false;
out.deviceInfos2.push_back(tmp);
}
}
node = node->next_sibling();
/* @generate [ deviceInfos2:std::vector< int > ] end @ */

/* @generate [ deviceInfo:ReqDeviceInfo ] begin @ */
if(!node || std::strcmp(node->name(), "deviceInfo"))
return false;
{
auto tmp = node->first_node();
if(!xml_struct(tmp, out.deviceInfo))
return false;
}
node = node->next_sibling();
/* @generate [ deviceInfo:ReqDeviceInfo ] end @ */

/* @generate [ time:int ] begin @ */
if(!node || std::strcmp(node->name(), "time"))
return false;
if(!xml_struct(node, out.time))
return false;
node = node->next_sibling();
/* @generate [ time:int ] end @ */

return true;
}


}
#endif // TEST_H
