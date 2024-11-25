#include <scwx/common/geographic.hpp>
#include <scwx/qt/model/marker_model.hpp>
#include <scwx/qt/manager/marker_manager.hpp>
#include <scwx/qt/types/marker_types.hpp>
#include <scwx/qt/types/qt_types.hpp>
#include <scwx/util/logger.hpp>

#include <vector>

#include <QApplication>

namespace scwx
{
namespace qt
{
namespace model
{

static const std::string logPrefix_ = "scwx::qt::model::marker_model";
static const auto        logger_    = scwx::util::Logger::Create(logPrefix_);

static constexpr int kFirstColumn =
   static_cast<int>(MarkerModel::Column::Latitude);
static constexpr int kLastColumn =
   static_cast<int>(MarkerModel::Column::Name);
static constexpr int kNumColumns = kLastColumn - kFirstColumn + 1;

class MarkerModel::Impl
{
public:
   explicit Impl() {}
   ~Impl() = default;
   std::shared_ptr<manager::MarkerManager> markerManager_ {
      manager::MarkerManager::Instance()};
   std::vector<types::MarkerId> markerIds_;
};

MarkerModel::MarkerModel(QObject* parent) :
   QAbstractTableModel(parent), p(std::make_unique<Impl>())
{

   connect(p->markerManager_.get(),
         &manager::MarkerManager::MarkersInitialized,
         this,
         &MarkerModel::HandleMarkersInitialized);

   connect(p->markerManager_.get(),
         &manager::MarkerManager::MarkerAdded,
         this,
         &MarkerModel::HandleMarkerAdded);

   connect(p->markerManager_.get(),
         &manager::MarkerManager::MarkerChanged,
         this,
         &MarkerModel::HandleMarkerChanged);

   connect(p->markerManager_.get(),
         &manager::MarkerManager::MarkerRemoved,
         this,
         &MarkerModel::HandleMarkerRemoved);
}

MarkerModel::~MarkerModel() = default;

int MarkerModel::rowCount(const QModelIndex& parent) const
{
   return parent.isValid() ?
             0 :
             static_cast<int>(p->markerIds_.size());
}

int MarkerModel::columnCount(const QModelIndex& parent) const
{
   return parent.isValid() ? 0 : kNumColumns;
}

Qt::ItemFlags MarkerModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = QAbstractTableModel::flags(index);

   switch (index.column())
   {
   case static_cast<int>(Column::Name):
   case static_cast<int>(Column::Latitude):
   case static_cast<int>(Column::Longitude):
      flags |= Qt::ItemFlag::ItemIsEditable;
      break;
   default:
      break;
   }

   return flags;
}

QVariant MarkerModel::data(const QModelIndex& index, int role) const
{

   static const char COORDINATE_FORMAT    = 'g';
   static const int  COORDINATE_PRECISION = 10;

   if (!index.isValid() || index.row() < 0 ||
       static_cast<size_t>(index.row()) >= p->markerIds_.size())
   {
      logger_->debug("Failed to get data index {}", index.row());
      return QVariant();
   }

   types::MarkerId id = p->markerIds_[index.row()];
   std::optional<types::MarkerInfo> markerInfo =
      p->markerManager_->get_marker(id);
   if (!markerInfo)
   {
      logger_->debug("Failed to get data index {} id {}", index.row(), id);
      return QVariant();
   }

   switch(index.column())
   {
   case static_cast<int>(Column::Name):
      if (role == Qt::ItemDataRole::DisplayRole ||
          role == Qt::ItemDataRole::ToolTipRole ||
          role == Qt::ItemDataRole::EditRole)
      {
         return QString::fromStdString(markerInfo->name);
      }
      break;

   case static_cast<int>(Column::Latitude):
      if (role == Qt::ItemDataRole::DisplayRole ||
          role == Qt::ItemDataRole::ToolTipRole)
      {
         return QString::fromStdString(
            common::GetLatitudeString(markerInfo->latitude));
      }
      else if (role == Qt::ItemDataRole::EditRole)
      {
         return QString::number(
            markerInfo->latitude, COORDINATE_FORMAT, COORDINATE_PRECISION);
      }
      break;

   case static_cast<int>(Column::Longitude):
      if (role == Qt::ItemDataRole::DisplayRole ||
          role == Qt::ItemDataRole::ToolTipRole)
      {
         return QString::fromStdString(
            common::GetLongitudeString(markerInfo->longitude));
      }
      else if (role == Qt::ItemDataRole::EditRole)
      {
         return QString::number(
            markerInfo->longitude, COORDINATE_FORMAT, COORDINATE_PRECISION);
      }
      break;
      break;

   default:
      break;
   }

   return QVariant();
}

std::optional<types::MarkerId> MarkerModel::getId(int index)
{
   if (index < 0 || static_cast<size_t>(index) >= p->markerIds_.size())
   {
      return {};
   }

   return p->markerIds_[index];
}

QVariant MarkerModel::headerData(int             section,
                                 Qt::Orientation orientation,
                                 int             role) const
{
   if (role == Qt::ItemDataRole::DisplayRole)
   {
      if (orientation == Qt::Horizontal)
      {
         switch (section)
         {
            case static_cast<int>(Column::Name):
               return tr("Name");

            case static_cast<int>(Column::Latitude):
               return tr("Latitude");

            case static_cast<int>(Column::Longitude):
               return tr("Longitude");

            default:
               break;
         }
      }
   }

   return QVariant();
}

bool MarkerModel::setData(const QModelIndex& index,
                          const QVariant&    value,
                          int                role)
{

   if (!index.isValid() || index.row() < 0 ||
       static_cast<size_t>(index.row()) >= p->markerIds_.size())
   {
      return false;
   }

   types::MarkerId id = p->markerIds_[index.row()];
   std::optional<types::MarkerInfo> markerInfo =
      p->markerManager_->get_marker(id);
   if (!markerInfo)
   {
      return false;
   }
   bool result = false;

   switch(index.column())
   {
   case static_cast<int>(Column::Name):
      if (role == Qt::ItemDataRole::EditRole)
      {
         QString str = value.toString();
         markerInfo->name = str.toStdString();
         p->markerManager_->set_marker(id, *markerInfo);
         result = true;
      }
      break;

   case static_cast<int>(Column::Latitude):
      if (role == Qt::ItemDataRole::EditRole)
      {
         QString str = value.toString();
         bool ok;
         double latitude = str.toDouble(&ok);
         if (!str.isEmpty() && ok && -90 <= latitude && latitude <= 90)
         {
            markerInfo->latitude = latitude;
            p->markerManager_->set_marker(id, *markerInfo);
            result = true;
         }
      }
      break;

   case static_cast<int>(Column::Longitude):
      if (role == Qt::ItemDataRole::EditRole)
      {
         QString str = value.toString();
         bool ok;
         double longitude = str.toDouble(&ok);
         if (!str.isEmpty() && ok && -180 <= longitude && longitude <= 180)
         {
            markerInfo->longitude = longitude;
            p->markerManager_->set_marker(id, *markerInfo);
            result = true;
         }
      }
      break;

   default:
      break;
   }

   if (result)
   {
      Q_EMIT dataChanged(index, index);
   }

   return result;
}

void MarkerModel::HandleMarkersInitialized(size_t count)
{
   if (count == 0)
   {
      return;
   }
   const int index = static_cast<int>(count - 1);

   p->markerIds_.reserve(count);
   beginInsertRows(QModelIndex(), 0, index);
   p->markerManager_->for_each(
      [this](const types::MarkerInfo& info)
      {
         p->markerIds_.push_back(info.id);
      });
   endInsertRows();
}

void MarkerModel::HandleMarkerAdded(types::MarkerId id)
{
   std::optional<size_t> index = p->markerManager_->get_index(id);
   const int newIndex = static_cast<int>(*index);

   beginInsertRows(QModelIndex(), newIndex, newIndex);
   auto it = std::next(p->markerIds_.begin(), newIndex);
   p->markerIds_.emplace(it, id);
   endInsertRows();
}

void MarkerModel::HandleMarkerChanged(types::MarkerId id)
{
   auto it = std::find(p->markerIds_.begin(), p->markerIds_.end(), id);
   if (it == p->markerIds_.end())
   {
      return;
   }
   const int changedIndex = std::distance(p->markerIds_.begin(), it);

   QModelIndex topLeft = createIndex(changedIndex, kFirstColumn);
   QModelIndex bottomRight = createIndex(changedIndex, kLastColumn);

   Q_EMIT dataChanged(topLeft, bottomRight);
}

void MarkerModel::HandleMarkerRemoved(types::MarkerId id)
{
   auto it = std::find(p->markerIds_.begin(), p->markerIds_.end(), id);
   if (it == p->markerIds_.end())
   {
      return;
   }

   const int removedIndex = std::distance(p->markerIds_.begin(), it);

   beginRemoveRows(QModelIndex(), removedIndex, removedIndex);
   p->markerIds_.erase(it);
   endRemoveRows();
}

} // namespace model
} // namespace qt
} // namespace scwx
