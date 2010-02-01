#include <Windows.h>
#include <tchar.h>
#include <Dbt.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>

#ifdef _MSC_VER
#  pragma comment(lib, "setupapi.lib")
#  pragma comment(lib, "cfgmgr32.lib")
#endif

#define SHINOBI_WINDOW_CLASS_NAME _T("ShinobiWindowClass")
#define BUFFER_SIZE 2048

/** GUID for USBSTOR interface class. . */
static const GUID GUID_DEVINTERFACE_USBSTOR = {0xA5DCBF10L, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

LRESULT ShinobiOnDeviceChange(WPARAM wParam, LPARAM lParam) {
  if(wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE) {
    PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR) lParam;
    switch(pHdr->dbch_devicetype) {
    case DBT_DEVTYP_DEVICEINTERFACE: {
      PDEV_BROADCAST_DEVICEINTERFACE pDevInf = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(pHdr);
      /*QString deviceName(pDevInf->dbcc_name);
      if(deviceName.startsWith("\\?\STORAGE#"))*/

      

      break;
    }
    case DBT_DEVTYP_HANDLE: {
      PDEV_BROADCAST_HANDLE pDevHnd = reinterpret_cast<PDEV_BROADCAST_HANDLE>(pHdr);
      break;
    }
    case DBT_DEVTYP_OEM: {
      PDEV_BROADCAST_OEM pDevOem = (PDEV_BROADCAST_OEM) pHdr;
      break;
    }
    case DBT_DEVTYP_PORT: {
      PDEV_BROADCAST_PORT pDevPort = (PDEV_BROADCAST_PORT) pHdr;
      break;
    }
    case DBT_DEVTYP_VOLUME: {
      PDEV_BROADCAST_VOLUME pDevVolume = (PDEV_BROADCAST_VOLUME) pHdr;
      break;
    }
    }
  }

  return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
  case WM_NCCREATE:
    return TRUE; /* To continue window creation. */
  case WM_DEVICECHANGE:
    return ShinobiOnDeviceChange(wParam, lParam);
  default:
    return 0;
  }
}

struct tagDrives
{
  WCHAR letter;
  WCHAR volume[BUFFER_SIZE];
} g_drives[26];

WCHAR GetSpecificDrive(LPTSTR lpDevID) {
  HDEVINFO hDevInfo;
  GUID guid;
  BYTE buffer[BUFFER_SIZE];
  DWORD dwRequiredSize ;
  WCHAR buf[BUFFER_SIZE];
  DEVINST devInstParent;
  DWORD dwIndex;
  WCHAR volume[BUFFER_SIZE];
  int nLength,nLoopIndex;
  SP_DEVICE_INTERFACE_DATA devInterfaceData;
  SP_DEVINFO_DATA devInfoData;
  PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetail;
  
  if(!lpDevID)
    return 0;

  // GUID_DEVINTERFACE_VOLUME is interface Guid for Volume class devices.
  guid = GUID_DEVINTERFACE_VOLUME;
  
  // Get device Information handle for Volume interface
  hDevInfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
  if(hDevInfo == INVALID_HANDLE_VALUE)
    return 0;

  // Loop until device interfaces are found.
  for(dwIndex = 0; ;dwIndex ++) {
    ZeroMemory(&devInterfaceData, sizeof(devInterfaceData));
    devInterfaceData.cbSize = sizeof(devInterfaceData);
    
    // Get device Interface data.
    if(!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &guid, dwIndex,&devInterfaceData))
      break;

    ZeroMemory(&devInfoData, sizeof(devInfoData));
    devInfoData.cbSize = sizeof(devInfoData);
    pDevDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) buffer;
    pDevDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    
    // Get device interface detail data to get
    // Device Instance from SP_DEVINFO_DATA and
    // Device Path from SP_DEVICE_INTERFACE_DETAIL_DATA
    SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, pDevDetail, BUFFER_SIZE, &dwRequiredSize, &devInfoData);
    
    // Get the device instance of parent. This points to USBSTOR.
    CM_Get_Parent(&devInstParent,devInfoData.DevInst, 0);
    
    // Get the device instance of grand parent. This points to USB root.
    CM_Get_Parent(&devInstParent,devInstParent, 0);
    
    // Get the device ID of the USB root.
    CM_Get_Device_ID(devInstParent, buf, BUFFER_SIZE,0);
    
    // If USB root device matches with the input device ID, it is the target device.
    if(buf != NULL && (wcscmp(lpDevID,buf) == 0) || wcscmp(buf, L"USB\\VID_05AC&PID_1209\\000A270015644F74") == 0) {

      // Append \ to the DevicePath of SP_DEVICE_INTERFACE_DETAIL_DATA
      nLength = wcslen(pDevDetail->DevicePath);
      pDevDetail->DevicePath[nLength] = '\\';
      pDevDetail->DevicePath[nLength+1] = 0;
      // Get Volume mount point for the device path.
      if(GetVolumeNameForVolumeMountPoint(pDevDetail->DevicePath, volume, BUFFER_SIZE)) {
        /*for(nLoopIndex=0; nLoopIndex< g_count; nLoopIndex++) {
          // Compare volume mount point with the one stored earlier.
          // If both match, return the corresponding drive letter.
          if(wcscmp(g_drives[nLoopIndex].volume, volume)==0) {
            SetupDiDestroyDeviceInfoList(hDevInfo);
            return g_drives[nLoopIndex].letter;
          }
        }*/
      }
    }
  }
  SetupDiDestroyDeviceInfoList(hDevInfo);
  return 0;
}

BOOL GetAllRemovableDisks() {
  WCHAR caDrive[4];
  WCHAR volume[BUFFER_SIZE];
  int nLoopIndex;
  DWORD dwDriveMask;
  caDrive[0] = 'A';
  caDrive[1] = ':';
  caDrive[2] = '\\';
  caDrive[3] = 0;
  int g_count = 0;
  // Get all drives in the system.
  dwDriveMask = GetLogicalDrives();
  if(dwDriveMask == 0)
  {
    //WRITELOG("Error - GetLogicalDrives failed\n");
    return FALSE;
  }
  // Loop for all drives (MAX_DRIVES = 26)
  int MAX_DRIVES = 26;
  for(nLoopIndex = 0; nLoopIndex< MAX_DRIVES; nLoopIndex++)
  {
    // if a drive is present,
    if(dwDriveMask & 1)
    {
      caDrive[0] = 'A' + nLoopIndex;
      // If a drive is removable
      if(GetDriveType(caDrive) == DRIVE_REMOVABLE)
      {
        //Get its volume info and store it in the global variable.
        if(GetVolumeNameForVolumeMountPoint(caDrive, volume, BUFFER_SIZE))
        {
          g_drives[g_count].letter = caDrive[0];
          wcscpy(g_drives[g_count].volume, volume);
          g_count ++;
        }
      }
    }
    dwDriveMask >>= 1;
  }
  // success if atleast one removable drive is found.
  if(g_count == 0)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  GUID guid = GUID_DEVINTERFACE_VOLUME;
  HDEVINFO hDevInfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
  if(hDevInfo == INVALID_HANDLE_VALUE)
    return 0;

  GetAllRemovableDisks();

  GetSpecificDrive(L"USB\Vid_05ac&Pid_1209");

  for(DWORD dwIndex = 0; ; dwIndex++) {
    SP_DEVICE_INTERFACE_DATA devInterfaceData;
    ZeroMemory(&devInterfaceData, sizeof(devInterfaceData));
    devInterfaceData.cbSize = sizeof(devInterfaceData);
    if(!SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &guid, dwIndex, &devInterfaceData))
      return 0;

    SP_DEVINFO_DATA devInfoData;
    ZeroMemory(&devInfoData, sizeof(devInfoData));
    devInfoData.cbSize = sizeof(devInfoData);

    CHAR devDetail[BUFFER_SIZE];
    PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetail = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(devDetail);
    pDevDetail->cbSize = BUFFER_SIZE;
    DWORD requiredSize;
    if(!SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, pDevDetail, BUFFER_SIZE, &requiredSize, &devInfoData)) {
      int n = GetLastError();
      return 0;
    }

    DEVINST devInstParent;
    CM_Get_Parent(&devInstParent, devInfoData.DevInst, 0); /* USBSTOR */
    CM_Get_Parent(&devInstParent, devInstParent, 0); /* USB */
    
    TCHAR devId[BUFFER_SIZE / sizeof(TCHAR)];
    CM_Get_Device_ID(devInstParent, devId, BUFFER_SIZE, 0);
    //if(_tcscmp(pDevInf->dbcc_name, devId) == 0) {
      /*
        // Append \ to the DevicePath of SP_DEVICE_INTERFACE_DETAIL_DATA
        nLength = wcslen(pDevDetail->DevicePath);
        pDevDetail->DevicePath[nLength] = '\\';
        pDevDetail->DevicePath[nLength+1] = 0;
        // Get Volume mount point for the device path.
        if(GetVolumeNameForVolumeMountPoint(pDevDetail->DevicePath, volume,
          BUFFER_SIZE))
        {
          for(nLoopIndex=0; nLoopIndex< g_count; nLoopIndex++)
          {
            // Compare volume mount point with the one stored earlier.
            // If both match, return the corresponding drive letter.
            if(wcscmp(g_drives[nLoopIndex].volume, volume)==0)
            {
              SetupDiDestroyDeviceInfoList(hDevInfo);
              return g_drives[nLoopIndex].letter;
            }
          }
        }
      }*/
      
    //}
    SetupDiDestroyDeviceInfoList(hDevInfo);
    //WRITELOG("Error - No drives found in GetSpecificDrives\n");
    return 0;
  }
  
  
  /* Register window class. */
  WNDCLASSEX wndClass;
  memset(&wndClass, 0, sizeof(WNDCLASSEX));
  wndClass.cbSize = sizeof(WNDCLASSEX);
  wndClass.lpfnWndProc    = WndProc;
  wndClass.hInstance      = hInstance;
  wndClass.lpszClassName  = SHINOBI_WINDOW_CLASS_NAME;
  if(!RegisterClassEx(&wndClass))
    return 0;

  /* Create window. */
  HWND hWnd = CreateWindow(SHINOBI_WINDOW_CLASS_NAME, 0, 0, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, 0, hInstance, 0);
  if(hWnd == NULL)
    return 0;

  /* Register for notifications. */
  DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
  ZeroMemory(&notificationFilter, sizeof(notificationFilter));
  notificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
  notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
  //notificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USBSTOR;
  HDEVNOTIFY hDevNotify = RegisterDeviceNotification(hWnd, &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
  if(!hDevNotify)
    return 0;

  /* Message loop. */
  MSG Msg;
  while(GetMessage(&Msg, NULL, 0, 0) != 0) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }
  return Msg.wParam;
}