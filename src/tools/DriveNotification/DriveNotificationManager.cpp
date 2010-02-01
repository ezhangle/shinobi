#include "DriveNotificationManager.h"
#include <QWriteLocker>

QReadWriteLock DriveNotificationManager::sLock(QReadWriteLock::NonRecursive);
DriveNotificationManager* DriveNotificationManager::sInstance = NULL;

DriveNotificationManager* DriveNotificationManager::instance() {
  QWriteLocker locker(&sLock);
  if(sInstance == NULL)
    sInstance = new DriveNotificationManager();
  return sInstance;
}
