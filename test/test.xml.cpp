/* @generate [ test:@namespace ] begin @ */
/*                                                    
                      _ooOoo_                         
                     o8888888o                        
                     88" . "88                        
                     (| -_- |)                        
                      O\ = /O                         
                  ____/`---'\____                     
                .   ' \\| |// `.                      
                 / \\||| : |||// \                    
               / _||||| -:- |||||- \                  
                 | | \\\ - /// | |                    
               | \_| ''\---/'' | |                    
                \ .-\__ `-` ___/-. /                  
             ___`. .' /--.--\ `. . __                 
          . '< `.___\_<|>_/___.' >'.              
         | | : `- \`.;`\ _ /`;.`/ - ` : | |           
           \ \ `-. \_ __\ /__ _/ .-` / /              
   ======`-.____`-.___\_____/___.-`____.-'======      
                      `=---='                         
                                                      
   .............................................      
              God bless        No bug                 
                 zwkno1@gmail.com                     
*/                                                    
/* @generate [ test:@namespace ] end @ */


#include "test.xml.h"
#include <cstring>
#include <sstream>


/* @generate [ test:@namespace ] begin @ */
namespace test
{

template<class T>bool xml_struct(const rapidxml::xml_node<> * node, T & out)
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

//for vector
template<class T>
bool struct_xml(rapidxml::xml_node<> * & node, const T & in, rapidxml::xml_document<> & doc)
{
    std::stringstream ss;
    ss << in;
    node = doc.allocate_node(rapidxml::node_element, "element", doc.allocate_string(ss.str().c_str()));
    return true;
}

// for base item
template<class T>
std::string get_string(const T & in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

} // namespace test

/* @generate [ test:@namespace ] end @ */

/* @generate [ test:@namespace ] begin @ */
namespace test
{
/* @generate [ ReqDeviceInfo:@entry ] begin @ */
bool struct_xml(rapidxml::xml_node<> * & node, const ReqDeviceInfo & in, rapidxml::xml_document<> & doc)
{
node = doc.allocate_node(rapidxml::node_element, doc.allocate_string("ReqDeviceInfo"));
/* @generate [ userId:int ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, "userId", doc.allocate_string(get_string(in.userId).c_str()));
node->append_node(node1);
}
/* @generate [ userId:int ] end @ */

/* @generate [ deviceId:int ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, "deviceId", doc.allocate_string(get_string(in.deviceId).c_str()));
node->append_node(node1);
}
/* @generate [ deviceId:int ] end @ */

return true;
}
/* @generate [ ReqDeviceInfo:@entry ] end @ */

/* @generate [ RetDeviceInfo:@entry ] begin @ */
bool struct_xml(rapidxml::xml_node<> * & node, const RetDeviceInfo & in, rapidxml::xml_document<> & doc)
{
node = doc.allocate_node(rapidxml::node_element, doc.allocate_string("RetDeviceInfo"));
/* @generate [ userId:int ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, "userId", doc.allocate_string(get_string(in.userId).c_str()));
node->append_node(node1);
}
/* @generate [ userId:int ] end @ */

/* @generate [ deviceId:int ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, "deviceId", doc.allocate_string(get_string(in.deviceId).c_str()));
node->append_node(node1);
}
/* @generate [ deviceId:int ] end @ */

/* @generate [ std::vector< ReqDeviceInfo >:ReqDeviceInfo ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("deviceInfos"));
node->append_node(node1);
auto node2 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("Vector"));
node1->append_node(node2);
for(auto & i : in.deviceInfos)
{
rapidxml::xml_node<> * tmp = 0;
if(!struct_xml(tmp, i, doc))
return false;
node2->append_node(tmp);
}
}
/* @generate [ std::vector< ReqDeviceInfo >:ReqDeviceInfo ] end @ */

/* @generate [ std::vector< int >:int ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("deviceInfos2"));
node->append_node(node1);
auto node2 = doc.allocate_node(rapidxml::node_element, doc.allocate_string("Vector"));
node1->append_node(node2);
for(auto & i : in.deviceInfos2)
{
rapidxml::xml_node<> * tmp = 0;
if(!struct_xml(tmp, i, doc))
return false;
node2->append_node(tmp);
}
}
/* @generate [ std::vector< int >:int ] end @ */

/* @generate [ deviceInfo:ReqDeviceInfo ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, "deviceInfo");
node->append_node(node1);
rapidxml::xml_node<> * node2 = 0;
if(!struct_xml(node2, in.deviceInfo, doc))
return false;
node1->append_node(node2);
}
/* @generate [ deviceInfo:ReqDeviceInfo ] end @ */

/* @generate [ time:int ] begin @ */
{
auto node1 = doc.allocate_node(rapidxml::node_element, "time", doc.allocate_string(get_string(in.time).c_str()));
node->append_node(node1);
}
/* @generate [ time:int ] end @ */

return true;
}
/* @generate [ RetDeviceInfo:@entry ] end @ */

} // namespace test

/* @generate [ test:@namespace ] end @ */

/* @generate [ test:@namespace ] begin @ */
namespace test
{
/* @generate [ ReqDeviceInfo:@entry ] begin @ */
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
/* @generate [ ReqDeviceInfo:@entry ] end @ */

/* @generate [ RetDeviceInfo:@entry ] begin @ */
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

/* @generate [ std::vector< ReqDeviceInfo >:ReqDeviceInfo ] begin @ */
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
/* @generate [ std::vector< ReqDeviceInfo >:ReqDeviceInfo ] end @ */

/* @generate [ std::vector< int >:int ] begin @ */
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
/* @generate [ std::vector< int >:int ] end @ */

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
/* @generate [ RetDeviceInfo:@entry ] end @ */

} // namespace test

/* @generate [ test:@namespace ] end @ */

