#include "DriveNotificationThread.h"
#include <Dbt.h>
#include "DriveNotificationManager.h"


#define SHINOBI_WINDOW_CLASS_NAME L"ShinobiWindowClass"

namespace shinobi {
  DriveNotificationThread* notificationThread = NULL;

  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
      case WM_DEVICECHANGE:
        assert(notificationThread != NULL);

        return notificationThread->onDeviceChange(wParam, lParam);
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
  }

// -------------------------------------------------------------------------- //
// DriveNotificationThread
// -------------------------------------------------------------------------- //
  DriveNotificationThread::DriveNotificationThread(): mHwnd(NULL), mValid(true), mLogicalDriveMask(GetLogicalDrives()) {
    /* Init drive descriptions. */
    /*mDriveDescriptions.resize(MAX_DRIVES);
    updateRemovableDrives();*/

    /* Get our instance. */
    HINSTANCE hInstance;
    if(!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, L"And what the hell are you expecting to find reading this executable?", &hInstance)) {
      mValid = false;
      return;
    }

    /* Register window class. */
    WNDCLASSEX wndClass;
    memset(&wndClass, 0, sizeof(WNDCLASSEX));
    wndClass.cbSize        = sizeof(WNDCLASSEX);
    wndClass.lpfnWndProc   = WndProc;
    wndClass.hInstance     = hInstance;
    wndClass.lpszClassName = SHINOBI_WINDOW_CLASS_NAME;
    if(!RegisterClassEx(&wndClass)) {
      mValid = false;
      return;
    }

    /* Create window. */
    mHwnd = CreateWindow(SHINOBI_WINDOW_CLASS_NAME, 0, 0, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, 0, hInstance, 0);
    if(mHwnd == NULL) {
      mValid = false;
      return;
    }

    /* Register for notifications. */
    DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
    ZeroMemory(&notificationFilter, sizeof(notificationFilter));
    notificationFilter.dbcc_size         = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    notificationFilter.dbcc_devicetype   = DBT_DEVTYP_DEVICEINTERFACE;
    /* notificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USBSTOR; */
    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(mHwnd, &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
    if(!hDevNotify) {
      mValid = false;
      return;
    }
  }


  void DriveNotificationThread::run() {
    /* Check validity. */
    if(!mValid)
      return;

    /* Message loop. */
    MSG Msg;
    while(GetMessage(&Msg, NULL, 0, 0) != 0) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
  }

  DriveNotificationThread::~DriveNotificationThread() {
    PostMessage(mHwnd, WM_DESTROY, 0, 0);
    wait();
  }

  LRESULT DriveNotificationThread::onDeviceChange(WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(wParam);

    DWORD logicalDriveMask = GetLogicalDrives();
    if(logicalDriveMask == mLogicalDriveMask)
      return TRUE;

    DWORD addedMask = logicalDriveMask & ~mLogicalDriveMask;
    DWORD removedMask = ~logicalDriveMask & mLogicalDriveMask;
    mLogicalDriveMask = logicalDriveMask;

    if(addedMask != 0) {
      for(int i = 0; i < MAX_DRIVES; i++) {
        if(addedMask & 1) {
          QString drivePathQStr = static_cast<char>('A' + i) + QString(":\\");
          QString driveNameQStr;
          QString serialNoQStr;

          WCHAR drivePath[4] = {'A' + i, ':', '\\', 0};
          WCHAR driveName[MAX_PATH + 1];
          WCHAR fileSystemName[MAX_PATH + 1];
          DWORD serialNo;
          if(GetVolumeInformation(drivePath, driveName, sizeof(driveName) / sizeof(WCHAR), &serialNo, NULL, NULL, fileSystemName, sizeof(fileSystemName) / sizeof(WCHAR))) {
            serialNoQStr = QString::number(serialNo, 16);
            driveNameQStr = QString::fromWCharArray(driveName);
          }

          notificationManager.notifyPlugged(drivePathQStr, serialNoQStr, driveNameQStr);
        }
        addedMask >>= 1;
      }
    }

    if(removedMask != 0) {
      for(int i = 0; i < MAX_DRIVES; i++) {
        if(removedMask & 1)
          notificationManager.notifyUnplugged(static_cast<char>('A' + i) + QString(":\\"));
        removedMask >>= 1;
      }
    }

    return TRUE;

    /*
    if(wParam != DBT_DEVICEARRIVAL && wParam != DBT_DEVICEREMOVECOMPLETE)
      return TRUE;

    PDEV_BROADCAST_HDR pHdr = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
    if(pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
      return TRUE;
    
    PDEV_BROADCAST_DEVICEINTERFACE pDevInf = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(pHdr);

    WCHAR storageName[] = L"\\\\?\\STORAGE";
    if(memcmp(pDevInf->dbcc_name, storageName, sizeof(storageName) - sizeof(WCHAR)) != 0)
      return TRUE;
    */
  }


} // namespace shinobi


#if 0
    struct DriveDescription {
      bool valid;
      WCHAR volumeId[BUFFER_SIZE];

      DriveDescription(): valid(false) {}
    };

    /** Updates removable drive descriptions.
     * 
     * @returns                       true if everything went fine, false otherwise. */
    bool updateRemovableDrives() {
      assert(mDriveDescriptions.size() == MAX_DRIVES);

      WCHAR drivePath[4] = {'A', ':', '\\', 0};
      
      /* Get all drives in the system. */
      DWORD dwDriveMask = GetLogicalDrives();
      if(dwDriveMask == 0)
        return false;

      /* Loop for all drives. */
      for(int i = 0; i < MAX_DRIVES; i++) {
        if((dwDriveMask & 1) && !mDriveDescriptions[i].valid) {
          drivePath[0] = 'A' + i;
          if(GetDriveType(drivePath) == DRIVE_REMOVABLE && GetVolumeNameForVolumeMountPoint(drivePath, mDriveDescriptions[i].volumeId, BUFFER_SIZE))
            mDriveDescriptions[i].valid = true;
        }
        if(!(dwDriveMask & 1) && mDriveDescriptions[i].valid)
          mDriveDescriptions[i].valid = false;
        dwDriveMask >>= 1;
      }

      return true;
    }
#endif