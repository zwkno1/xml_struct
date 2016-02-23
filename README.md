# xml_struct
神奇的代码
自动生成xml与结构体之间的转换的代码
举个镊子

定义如下的xml：
<struct name='ReqDeviceInfo' type='struct' def='struct' desc='qing qiu shebei xingxi'>
<item name='userId' type='base' def='int' desc='yong hu id'></item>
<item name='deviceId' type='base' def='int' desc='she bei id'></item>
</struct>
<struct name='RetDeviceInfo' type='struct' def='struct' desc='fan hui shebei xingxi'> 
<item name='userId' type='base' def='int' desc='yong hu id'></item>
<item name='deviceId' type='base' def='int' desc='she bei id'></item>
<item name='deviceInfo' type='seq' def='std::vector<ReqDeviceInfo>' base_type='ReqDeviceInfo' desc='she bei zhuang tai'></item>
</struct>

生成如下代码：
目前只有xml解析出struct
struct 生成xml的 这个函数 xml_put还没完成，有空的大神可以帮忙完成
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

