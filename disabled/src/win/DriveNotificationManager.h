#ifndef __SHINOBI_DRIVE_NOTIFICATION_MANAGER_H__
#define __SHINOBI_DRIVE_NOTIFICATION_MANAGER_H__

#include "config.h"
#include <QSet>
#include "ScopedLock.h"
#include "../DriveNotifier.h"
#include "DriveNotificationThread.h"

namespace shinobi {
// -------------------------------------------------------------------------- //
// DriveNotificationManager
// -------------------------------------------------------------------------- //
  /** Main class for managing storage notifications. */
  class DriveNotificationManager {
  public:
    DriveNotificationManager() {}

    void addNotifier(DriveNotifier* notificator) {
      ScopedLock lock(mNotifiersMutex);
      if(mStorageNotificators.size() == 0) {
        notificationThread = new DriveNotificationThread();
        notificationThread->start();
      }
      mStorageNotificators.insert(notificator);
    }

    void removeNotifier(DriveNotifier* notificator) {
      ScopedLock lock(mNotifiersMutex);
      mStorageNotificators.remove(notificator);
      if(mStorageNotificators.size() == 0) {
        delete notificationThread;
        notificationThread = NULL;
      }
    }

    void notifyPlugged(const QString& path, const QString& serialNo, const QString& name) {
      ScopedLock lock(mNotifiersMutex);
      foreach(DriveNotifier* notifier, mStorageNotificators)
        notifier->emitDrivePlugged(path, serialNo, name);
    }

    void notifyUnplugged(const QString& path) {
      ScopedLock lock(mNotifiersMutex);
      foreach(DriveNotifier* notifier, mStorageNotificators)
        notifier->emitDriveUnplugged(path);
    }

  private:
    DriveNotificationManager(const DriveNotificationManager&);
    DriveNotificationManager& operator= (const DriveNotificationManager&);

    QMutex mNotifiersMutex;
    QSet<DriveNotifier*> mStorageNotificators;
  };

  extern DriveNotificationManager notificationManager;

} // namespace shinobi

#endif // __SHINOBI_DRIVE_NOTIFICATION_MANAGER_H__
