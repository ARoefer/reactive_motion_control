// Generated by gencpp from file giskard_high/FollowPath.msg
// DO NOT EDIT!


#ifndef GISKARD_HIGH_MESSAGE_FOLLOWPATH_H
#define GISKARD_HIGH_MESSAGE_FOLLOWPATH_H

#include <ros/service_traits.h>


#include <giskard_high/FollowPathRequest.h>
#include <giskard_high/FollowPathResponse.h>


namespace giskard_high
{

struct FollowPath
{

typedef FollowPathRequest Request;
typedef FollowPathResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct FollowPath
} // namespace giskard_high


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::giskard_high::FollowPath > {
  static const char* value()
  {
    return "201a50976823ee0afe36476bd2e9a0a7";
  }

  static const char* value(const ::giskard_high::FollowPath&) { return value(); }
};

template<>
struct DataType< ::giskard_high::FollowPath > {
  static const char* value()
  {
    return "giskard_high/FollowPath";
  }

  static const char* value(const ::giskard_high::FollowPath&) { return value(); }
};


// service_traits::MD5Sum< ::giskard_high::FollowPathRequest> should match 
// service_traits::MD5Sum< ::giskard_high::FollowPath > 
template<>
struct MD5Sum< ::giskard_high::FollowPathRequest>
{
  static const char* value()
  {
    return MD5Sum< ::giskard_high::FollowPath >::value();
  }
  static const char* value(const ::giskard_high::FollowPathRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::giskard_high::FollowPathRequest> should match 
// service_traits::DataType< ::giskard_high::FollowPath > 
template<>
struct DataType< ::giskard_high::FollowPathRequest>
{
  static const char* value()
  {
    return DataType< ::giskard_high::FollowPath >::value();
  }
  static const char* value(const ::giskard_high::FollowPathRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::giskard_high::FollowPathResponse> should match 
// service_traits::MD5Sum< ::giskard_high::FollowPath > 
template<>
struct MD5Sum< ::giskard_high::FollowPathResponse>
{
  static const char* value()
  {
    return MD5Sum< ::giskard_high::FollowPath >::value();
  }
  static const char* value(const ::giskard_high::FollowPathResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::giskard_high::FollowPathResponse> should match 
// service_traits::DataType< ::giskard_high::FollowPath > 
template<>
struct DataType< ::giskard_high::FollowPathResponse>
{
  static const char* value()
  {
    return DataType< ::giskard_high::FollowPath >::value();
  }
  static const char* value(const ::giskard_high::FollowPathResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // GISKARD_HIGH_MESSAGE_FOLLOWPATH_H
