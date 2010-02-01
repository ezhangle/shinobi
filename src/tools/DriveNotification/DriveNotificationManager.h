#ifndef __DRIVE_NOTIFIER_H__
#define __DRIVE_NOTIFIER_H__

#include <QObject>
#include <QString>
#include <QReadWriteLock>

// -------------------------------------------------------------------------- //
// DriveNotificationManager
// -------------------------------------------------------------------------- //
class DriveNotificationManager: public QObject {
  Q_OBJECT;
public:
  enum StateChange {
    Plugged,
    Unplugged
  };

  static DriveNotificationManager* instance();

Q_SIGNALS:
  void driveStateChanged(int stateChange, QString path, QString name, QString serialNo);

private:
  DriveNotificationManager();
  ~DriveNotificationManager();

  static QReadWriteLock sLock;
  static DriveNotificationManager* sInstance;

  class DriveEventTrigger;
  DriveEventTrigger* mDriveEventTrigger;
};

#endif // __DRIVE_NOTIFIER_H__
