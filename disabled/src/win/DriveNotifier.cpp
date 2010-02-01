#include "../DriveNotifier.h"
#include <QMutex>
#include "DriveNotificationManager.h"
#include "DriveNotificationThread.h"

#ifndef UNICODE
#  error DriveNotifier must be compiled in UNICODE mode.
#endif

namespace shinobi {
  /* Allocate globals. */
  DriveNotificationManager notificationManager;

// -------------------------------------------------------------------------- //
// DriveNotifier
// -------------------------------------------------------------------------- //
  DriveNotifier::DriveNotifier(QObject* parent): QObject(parent) {
    notificationManager.addNotifier(this);
  }

  DriveNotifier::~DriveNotifier() {
    notificationManager.removeNotifier(this);
  }

} // namespace shinobi
