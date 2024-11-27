#pragma once

#include <string>
#include <cstdint>

namespace scwx
{
namespace qt
{
namespace types
{
typedef std::uint64_t MarkerId;

struct MarkerInfo
{
   MarkerInfo(const std::string& name, double latitude, double longitude) :
       name {name}, latitude {latitude}, longitude {longitude}
   {
   }

   MarkerId    id;
   std::string name;
   double      latitude;
   double      longitude;
};

} // namespace types
} // namespace qt
} // namespace scwx
