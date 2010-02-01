#ifndef __SHINOBI_DRIVE_NOTIFICATION_THREAD_H__
#define __SHINOBI_DRIVE_NOTIFICATION_THREAD_H__

#include "config.h"
#include <cassert>
#include <QThread>
#include <QVector>
#include <Windows.h>
#include "../DriveNotifier.h"

namespace shinobi {
// -------------------------------------------------------------------------- //
// DriveNotificationThread
// -------------------------------------------------------------------------- //
  class DriveNotificationThread: public QThread {
    Q_OBJECT;

    enum { MAX_DRIVES = 26 };

  public:
    DriveNotificationThread();

    virtual void run();

    ~DriveNotificationThread();

  private:
    friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    /** Handler for WM_DEVICECHANGE message. */
    LRESULT onDeviceChange(WPARAM wParam, LPARAM lParam);

    DWORD mLogicalDriveMask;
    HWND mHwnd;
    bool mValid;
  };

  extern DriveNotificationThread* notificationThread;

} // namespace shinobi

#endif // __SHINOBI_DRIVE_NOTIFICATION_THREAD_H__
