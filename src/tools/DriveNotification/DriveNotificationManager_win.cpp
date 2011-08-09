#include "DriveNotificationManager.h"
#include <cassert>
#include <QWidget>
#include <QReadLocker>

#include <Windows.h>
#include <Dbt.h>
#include <Cfgmgr32.h>
#include <Setupapi.h>

class DriveNotificationManager::DriveEventTrigger: public QWidget {
public:
  DriveEventTrigger() {
    /* Register for notifications. */
    DEV_BROADCAST_DEVICEINTERFACE_W notificationFilter;
    ZeroMemory(&notificationFilter, sizeof(notificationFilter));
    notificationFilter.dbcc_size         = sizeof(notificationFilter);
    notificationFilter.dbcc_devicetype   = DBT_DEVTYP_DEVICEINTERFACE;
    mDevNotify = RegisterDeviceNotificationW(winId(), &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);

    /* Store current drive mask. */
    mLogicalDriveMask = GetLogicalDrives();
  }

  ~DriveEventTrigger() {
    if(mDevNotify != NULL)
      UnregisterDeviceNotification(mDevNotify);
  }

  virtual bool winEvent(MSG* m, long* result)
  {
    if(m->message == WM_DEVICECHANGE) {
      *result = onDeviceChange(m);
      return true;
    } else
      return QWidget::winEvent(m, result);
  }

private:
  enum { MAX_DRIVES = 26 };

  long onDeviceChange(MSG* m) {
    assert(m->message == WM_DEVICECHANGE);

    /* Skip messages that we're not interested in. */
    if(m->wParam != DBT_DEVICEARRIVAL && m->wParam != DBT_DEVICEREMOVECOMPLETE)
      return TRUE;

    /* Skip non-interface messages. Well, interface messages seem to be
     * the only kind of messages we're receiving, but we still need to be
     * cautious. */
    PDEV_BROADCAST_DEVICEINTERFACE pDevInf = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(m->lParam);
    if(pDevInf->dbcc_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
      return TRUE;

    /* Skip messages that don't add/remove actual drives. */
    DWORD logicalDriveMask = GetLogicalDrives();
    if(logicalDriveMask == mLogicalDriveMask)
      return TRUE;

    /* Query device info from the system. */
    QString path, name, serialNo;
    queryDriveParams(pDevInf, path, name, serialNo);
    if(path.isNull())
      return TRUE;

    //DWORD deltaMask = logicalDriveMask ^ mLogicalDriveMask;
    mLogicalDriveMask = logicalDriveMask;

#if 0
    /* In case we got nothing - get at least path. */
    if(path.isEmpty()) {
      for(int i = 0; i < MAX_DRIVES; i++) {
        if(deltaMask & 1) {
          path = static_cast<char>('A' + i) + QString(":\\");
          break;
        }
      }
    }
#endif

    Q_EMIT DriveNotificationManager::sInstance->driveStateChanged(m->wParam == DBT_DEVICEARRIVAL ? Plugged : Unplugged, path, name, serialNo);
    return TRUE;
  }

  void queryDriveParams(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, QString& drivePathQStr, QString& driveNameQStr, QString& serialNoQStr) {
    /* Get path. */
    drivePathFromRegistry(pDevInf, drivePathQStr);

    /* Get name & serial. */
    driveNameAndSerialFromSetupApi(pDevInf, driveNameQStr, serialNoQStr);

    /* Try to get it another way. */
    if(driveNameQStr.isEmpty() || serialNoQStr.isEmpty())
      driveNameAndSerialFromRegistry(pDevInf, driveNameQStr, serialNoQStr);
  }

  void driveNameAndSerialFromSetupApi(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, QString& driveNameQStr, QString& serialNoQStr) {
    /* Construct device instance id. */
    QString deviceIdQStr = QString::fromWCharArray(pDevInf->dbcc_name);

    QRegExp deviceIdRegExp("^\\\\\\\\\\?\\\\.*#\\{........-....-....-....-............\\}$");
    if(!deviceIdRegExp.exactMatch(deviceIdQStr))
      return;
    
    /* Remove trailing GUIDs. */
    do {
      deviceIdQStr.chop(39);
    } while(deviceIdRegExp.exactMatch(deviceIdQStr));
    
    /* Clean it up so that setup API will understand it. */
    deviceIdQStr.replace('#', '\\').remove(0, 4);

    /* Convert id to WCHAR array. */
    WCHAR deviceId[1024];
    if(deviceIdQStr.size() > sizeof(deviceId) / sizeof(WCHAR))
      return;
    deviceId[deviceIdQStr.toWCharArray(deviceId)] = 0;

    /* Device id of the USBSTOR node. */
    WCHAR usbStorDeviceId[1024];

    /* Get device instance handle of the storage device (STORAGE). */
    if(deviceIdQStr.startsWith("STORAGE")) {
      DEVINST devInst;
      if(CM_Locate_DevNodeW(&devInst, deviceId, CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS) {
        return; 
      } else {
        /* Get parent device instance handle (USBSTOR). */
        DEVINST parentDevInst;
        if(CM_Get_Parent(&parentDevInst, devInst, 0) != CR_SUCCESS) {
          return;
        } else {
          /* Get parent device id. */
          if(CM_Get_Device_IDW(parentDevInst, usbStorDeviceId, sizeof(usbStorDeviceId) / sizeof(WCHAR), 0) != CR_SUCCESS)
            return;
        }
      }
    } else if(deviceIdQStr.startsWith("USBSTOR")) {
      usbStorDeviceId[deviceIdQStr.toWCharArray(usbStorDeviceId)] = 0;
    } else {
      return;
    }

    /* Convert to QString and extract serial number. */
    QString usbStorDeviceIdQStr = QString::fromWCharArray(usbStorDeviceId);
    if(!usbStorDeviceIdQStr.startsWith("USBSTOR\\"))
      return;
    serialNoQStr = usbStorDeviceIdQStr.mid(usbStorDeviceIdQStr.lastIndexOf('\\') + 1);
    serialNoQStr = serialNoQStr.left(serialNoQStr.indexOf('&'));

    /* Get user-friendly name. */
    GUID guid = GUID_DEVINTERFACE_DISK;
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsW(&guid, NULL, NULL, DIGCF_DEVICEINTERFACE);
    if(deviceInfoSet == INVALID_HANDLE_VALUE)
      return;

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    if(!SetupDiOpenDeviceInfoW(deviceInfoSet, usbStorDeviceId, NULL, 0, &devInfoData)) {
      SetupDiDestroyDeviceInfoList(deviceInfoSet);
      return;
    }

    WCHAR userFriendlyName[1024];
    if(!SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, &devInfoData, SPDRP_FRIENDLYNAME, NULL, reinterpret_cast<PBYTE>(userFriendlyName), sizeof(userFriendlyName), NULL)) {
      SetupDiDestroyDeviceInfoList(deviceInfoSet);
      return;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    driveNameQStr = QString::fromWCharArray(userFriendlyName);
    return;
  }

  void driveNameAndSerialFromRegistry(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, QString& driveNameQStr, QString& serialNoQStr) {
    /* Construct parent id. */
    QString parentIdQStr = QString::fromWCharArray(pDevInf->dbcc_name);
    int lastSharp = parentIdQStr.lastIndexOf('#');
    int preLastSharp = parentIdQStr.lastIndexOf('#', lastSharp - 1);
    parentIdQStr = parentIdQStr.mid(preLastSharp + 1, lastSharp - preLastSharp - 1);
    parentIdQStr = parentIdQStr.left(parentIdQStr.lastIndexOf('&'));
    
    WCHAR parentId[1024];
    parentId[parentIdQStr.toWCharArray(parentId)] = 0;

    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
      return;

    WCHAR subKey[2048];
    DWORD subKeySize;
    for(DWORD index = 0; RegEnumKeyExW(hKey, index, subKey, (subKeySize = sizeof(subKey) / sizeof(WCHAR), &subKeySize), NULL, NULL, NULL, NULL) == ERROR_SUCCESS; index++) {
      HKEY hSubKey;
      if(RegOpenKeyExW(hKey, subKey, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS)
        continue;

      WCHAR subSubKey[2048];
      DWORD subSubKeySize = sizeof(subSubKey) / sizeof(WCHAR);
      if(RegEnumKeyExW(hSubKey, 0, subSubKey, &subSubKeySize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
        RegCloseKey(hSubKey);
        continue;
      }

      HKEY hSubSubKey;
      if(RegOpenKeyExW(hSubKey, subSubKey, 0, KEY_READ, &hSubSubKey) != ERROR_SUCCESS) {
        RegCloseKey(hSubKey);
        continue;
      }
      RegCloseKey(hSubKey);

      BYTE data[4096];
      LPWSTR wData = reinterpret_cast<LPWSTR>(data);
      DWORD dataSize;
      DWORD regType;
      if(RegQueryValueExW(hSubSubKey, L"ParentIdPrefix", NULL, &regType, data, (dataSize = sizeof(data), &dataSize)) != ERROR_SUCCESS || regType != REG_SZ || wcscmp(wData, parentId) != 0) {
        RegCloseKey(hSubSubKey);
        continue;
      }

      serialNoQStr = QString::fromWCharArray(subSubKey);
      serialNoQStr = serialNoQStr.left(serialNoQStr.indexOf('&'));

      if(RegQueryValueExW(hSubSubKey, L"FriendlyName", NULL, &regType, data, (dataSize = sizeof(data), &dataSize)) == ERROR_SUCCESS && regType == REG_SZ) {
        driveNameQStr = QString::fromWCharArray(wData);
        RegCloseKey(hKey);
        RegCloseKey(hSubSubKey);
        return;
      }
      RegCloseKey(hSubSubKey);
    }

    RegCloseKey(hKey);
  }

  void drivePathFromRegistry(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, QString& drivePathQStr) {
    HKEY hKey;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\MountedDevices", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
      return;

    WCHAR subKey[2048];
    DWORD subKeySize;
    BYTE data[4096];
    LPWSTR wData = reinterpret_cast<LPWSTR>(data);
    DWORD dataSize;
    DWORD regType;
    for(DWORD index = 0; RegEnumValueW(hKey, index, subKey, (subKeySize = sizeof(subKey) / sizeof(WCHAR), &subKeySize), NULL, &regType, data, (dataSize = sizeof(data), &dataSize)) == ERROR_SUCCESS; index++) {
      if(regType != REG_BINARY && regType != REG_SZ)
        continue;

      if(regType == REG_BINARY)
        wData[dataSize / sizeof(WCHAR)] = 0;

      if(wcscmp(pDevInf->dbcc_name + 4, wData + 4) == 0 && QString::fromWCharArray(subKey).startsWith("\\DosDevices")) {
        drivePathQStr = QString::fromWCharArray(subKey).right(2) + "\\";
        RegCloseKey(hKey);
        return;
      }
    }

    RegCloseKey(hKey);
  }

  HDEVNOTIFY mDevNotify;
  DWORD mLogicalDriveMask;
};

DriveNotificationManager::DriveNotificationManager() {
  mDriveEventTrigger = new DriveEventTrigger();
}

DriveNotificationManager::~DriveNotificationManager() {
  delete mDriveEventTrigger;
}
