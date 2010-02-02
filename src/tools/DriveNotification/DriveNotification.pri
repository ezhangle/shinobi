HEADERS     += $$PWD/DriveNotificationManager.h
SOURCES     += $$PWD/DriveNotificationManager.cpp
INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

win32: {
  SOURCES += $$PWD/DriveNotificationManager_win.cpp
  LIBS    += setupapi.lib
}
