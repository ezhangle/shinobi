#ifndef __SHINOBI_HISTORY_ITEM_H__
#define __SHINOBI_HISTORY_ITEM_H__

#include "config.h"
#include <cassert>
#include <QMetaType>
#include <QString>
#include <QDateTime>
#include <DriveNotificationManager.h>

namespace shinobi {
// -------------------------------------------------------------------------- //
// HistoryItem
// -------------------------------------------------------------------------- //
  class HistoryItem {
  public:
    static QString eventTypeToString(DriveNotificationManager::StateChange stateChange) {
      switch(stateChange) {
      case DriveNotificationManager::Plugged:
        return "Plugged";
      case DriveNotificationManager::Unplugged:
        return "Unplugged";
      default:
        Unreachable();
      }
    }

    HistoryItem(): mEventType(DriveNotificationManager::Plugged) {}

    HistoryItem(DriveNotificationManager::StateChange type, const QDateTime& time, const QString& path, const QString& name, const QString& serialNo):
      mEventType(type), mTime(time), mPath(path), mName(name), mSerialNo(serialNo) {}

    DriveNotificationManager::StateChange eventType() const {
      return mEventType;
    }

    QDateTime time() const { 
      return mTime; 
    }

    QString path() const { 
      return mPath; 
    }

    QString name() const {
      return mName; 
    }

    QString serialNo() const { 
      return mSerialNo;
    }

    friend QDataStream &operator<<(QDataStream &out, const HistoryItem &filter) {
      return out << static_cast<int>(filter.mEventType) << filter.mTime << filter.mPath << filter.mName << filter.mSerialNo;
    }

    friend QDataStream &operator>>(QDataStream &in, HistoryItem &filter) {
      int type;
      in >> type >> filter.mTime >> filter.mPath >> filter.mName >> filter.mSerialNo;
      filter.mEventType = DriveNotificationManager::StateChange(type);
      return in;
    }

  private:
    static int registerInQtTypeSystem();

    DriveNotificationManager::StateChange mEventType;
    QDateTime mTime;
    QString mPath, mName, mSerialNo;
    static int sQtMetaTypeId;
  };

} // namespace shinobi

Q_DECLARE_METATYPE(::shinobi::HistoryItem);

#endif // __SHINOBI_HISTORY_ITEM_H__
