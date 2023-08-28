#include <scwx/qt/util/maplibre.hpp>

#include <QMapLibreGL/utils.hpp>
#include <mbgl/util/constants.hpp>

namespace scwx
{
namespace qt
{
namespace util
{
namespace maplibre
{

units::length::meters<double>
GetMapDistance(const QMapLibreGL::CustomLayerRenderParameters& params)
{
   return units::length::meters<double>(
      QMapLibreGL::metersPerPixelAtLatitude(params.latitude, params.zoom) *
      (params.width + params.height) / 2.0);
}

glm::mat4 GetMapMatrix(const QMapLibreGL::CustomLayerRenderParameters& params)
{
   glm::vec2 scale = GetMapScale(params);

   glm::mat4 mapMatrix(1.0f);
   mapMatrix = glm::scale(mapMatrix, glm::vec3(scale, 1.0f));
   mapMatrix = glm::rotate(mapMatrix,
                           glm::radians<float>(params.bearing),
                           glm::vec3(0.0f, 0.0f, 1.0f));

   return mapMatrix;
}

glm::vec2 GetMapScale(const QMapLibreGL::CustomLayerRenderParameters& params)
{
   const float scale = std::pow(2.0, params.zoom) * 2.0f *
                       mbgl::util::tileSize_D / mbgl::util::DEGREES_MAX;
   const float xScale = scale / params.width;
   const float yScale = scale / params.height;

   return glm::vec2 {xScale, yScale};
}

glm::vec2 LatLongToScreenCoordinate(const QMapLibreGL::Coordinate& coordinate)
{
   static constexpr double RAD2DEG_D = 180.0 / M_PI;

   double latitude = std::clamp(
      coordinate.first, -mbgl::util::LATITUDE_MAX, mbgl::util::LATITUDE_MAX);
   glm::vec2 screen {
      mbgl::util::LONGITUDE_MAX + coordinate.second,
      -(mbgl::util::LONGITUDE_MAX -
        RAD2DEG_D *
           std::log(std::tan(M_PI / 4.0 +
                             latitude * M_PI / mbgl::util::DEGREES_MAX)))};
   return screen;
}

} // namespace maplibre
} // namespace util
} // namespace qt
} // namespace scwx
