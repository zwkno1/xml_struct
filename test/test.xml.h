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



#include <rapidxml.h>

#include <vector>


/* @generate [ test:@namespace ] begin @ */
namespace test
{
/* @generate [ ReqDeviceInfo:@entry ] begin @ */
// qing qiu shebei xingxi
struct ReqDeviceInfo
{
/* @generate [ userId:int ] begin @ */
// yong hu id
int    userId;

/* @generate [ userId:int ] end @ */

/* @generate [ deviceId:int ] begin @ */
// she bei id
int    deviceId;

/* @generate [ deviceId:int ] end @ */

};

bool xml_struct(const rapidxml::xml_node<> * , ReqDeviceInfo & );
bool struct_xml(rapidxml::xml_node<> * &, const ReqDeviceInfo & ,rapidxml::xml_document<> & );
/* @generate [ ReqDeviceInfo:@entry ] end @ */

/* @generate [ RetDeviceInfo:@entry ] begin @ */
// fan hui shebei xingxi
struct RetDeviceInfo
{
/* @generate [ userId:int ] begin @ */
// yong hu id
int    userId;

/* @generate [ userId:int ] end @ */

/* @generate [ deviceId:int ] begin @ */
// she bei id
int    deviceId;

/* @generate [ deviceId:int ] end @ */

/* @generate [ std::vector< ReqDeviceInfo >:ReqDeviceInfo ] begin @ */
// she bei zhuang tai
::std::vector< ReqDeviceInfo >    deviceInfos;
/* @generate [ std::vector< ReqDeviceInfo >:ReqDeviceInfo ] end @ */

/* @generate [ std::vector< int >:int ] begin @ */
// she bei zhuang tai
::std::vector< int >    deviceInfos2;
/* @generate [ std::vector< int >:int ] end @ */

/* @generate [ deviceInfo:ReqDeviceInfo ] begin @ */
// she bei zhuang tai
ReqDeviceInfo    deviceInfo;

/* @generate [ deviceInfo:ReqDeviceInfo ] end @ */

/* @generate [ time:int ] begin @ */
// she bei shi jian
int    time;

/* @generate [ time:int ] end @ */

};

bool xml_struct(const rapidxml::xml_node<> * , RetDeviceInfo & );
bool struct_xml(rapidxml::xml_node<> * &, const RetDeviceInfo & ,rapidxml::xml_document<> & );
/* @generate [ RetDeviceInfo:@entry ] end @ */

} // namespace test

/* @generate [ test:@namespace ] end @ */

