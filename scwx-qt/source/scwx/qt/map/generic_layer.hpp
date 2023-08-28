#pragma once

#include <scwx/qt/map/map_context.hpp>

#include <memory>

#include <QObject>
#include <QMapLibreGL/QMapLibreGL>
#include <glm/gtc/type_ptr.hpp>

namespace scwx
{
namespace qt
{
namespace map
{

class GenericLayerImpl;

class GenericLayer : public QObject
{
   Q_OBJECT

public:
   explicit GenericLayer(std::shared_ptr<MapContext> context);
   virtual ~GenericLayer();

   virtual void Initialize()                                            = 0;
   virtual void Render(const QMapLibreGL::CustomLayerRenderParameters&) = 0;
   virtual void Deinitialize()                                          = 0;

   /**
    * @brief Run mouse picking on the layer.
    *
    * @param [in] params Custom layer render parameters
    * @param [in] mousePos Mouse cursor location in map screen coordinates
    *
    * @return true if a draw item was picked, otherwise false
    */
   virtual bool
   RunMousePicking(const QMapLibreGL::CustomLayerRenderParameters& params,
                   const glm::vec2&                                mousePos);

protected:
   std::shared_ptr<MapContext> context() const;

private:
   std::unique_ptr<GenericLayerImpl> p;
};

} // namespace map
} // namespace qt
} // namespace scwx
