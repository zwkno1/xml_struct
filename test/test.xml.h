#pragma once

#include <vector>
#include <string>

#include <rapidxml.hpp>


/* @generate [ test:@namespace ] begin @ */

namespace test
{
/* @generate [ ReqDeviceInfo:struct ] begin @ */

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

/* @generate [ ReqDeviceInfo:struct ] end @ */

/* @generate [ RetDeviceInfo:struct ] begin @ */

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

/* @generate [ deviceInfos:std::vector< ReqDeviceInfo > ] begin @ */

// she bei zhuang tai
std::vector< ReqDeviceInfo >    deviceInfos;

/* @generate [ deviceInfos:std::vector< ReqDeviceInfo > ] end @ */

/* @generate [ deviceInfos2:std::vector< int > ] begin @ */

// she bei zhuang tai
std::vector< int >    deviceInfos2;

/* @generate [ deviceInfos2:std::vector< int > ] end @ */

/* @generate [ deviceInfo:ReqDeviceInfo ] begin @ */

// she bei zhuang tai
ReqDeviceInfo    deviceInfo;

/* @generate [ deviceInfo:ReqDeviceInfo ] end @ */

/* @generate [ time:int ] begin @ */

// she bei shi jian
int    time;

/* @generate [ time:int ] end @ */

};

/* @generate [ RetDeviceInfo:struct ] end @ */

} // namespace test

bool xml_struct(const rapidxml::xml_node<> * , test::ReqDeviceInfo & );
bool struct_xml(rapidxml::xml_node<> * &, const test::ReqDeviceInfo & ,rapidxml::xml_document<> & );

bool xml_struct(const rapidxml::xml_node<> * , test::RetDeviceInfo & );
bool struct_xml(rapidxml::xml_node<> * &, const test::RetDeviceInfo & ,rapidxml::xml_document<> & );

/* @generate [ test:@namespace ] end @ */

