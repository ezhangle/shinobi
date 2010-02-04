TEMPLATE  = app
CONFIG   += qt warn_on

SOURCES = \
  src/DriveFilter.cpp \
  src/FileFilter.cpp \
  src/HistoryItem.cpp \
  src/KeySequenceEdit.cpp \
  src/main.cpp \
  src/MainWidget.cpp \
  src/Shinobi.cpp \
  src/WorkerThread.cpp \
  
HEADERS = \
  src/config.h \
  src/DriveFilter.h \
  src/FileFilter.h \
  src/FileFilterActionDelegate.h \
  src/FileFilterActionEditor.h \
  src/HistoryItem.h \
  src/KeySequenceEdit.h \
  src/Shinobi.h \
  src/MainWidget.h \
  src/WorkerThread.h \

include(src/3rdparty/globalshortcut/globalshortcut.pri)
include(src/tools/DriveNotification/DriveNotification.pri)

FORMS = 

RESOURCES = \
  src/res/shinobi.qrc \

UI_DIR    = src/ui
MOC_DIR   = bin/temp/moc
RCC_DIR   = bin/temp/rcc
TARGET    = shinobi

win32:RC_FILE    = src/res/shinobi.rc

win32:debug {
  DESTDIR         = bin/debug
  OBJECTS_DIR     = bin/debug
  CONFIG         += console
}

win32:release {
  DESTDIR         = bin/release
  OBJECTS_DIR     = bin/release
  CONFIG         -= console
  QMAKE_POST_LINK = upx -9 -q $$DESTDIR/$$join(TARGET, "", "", ".exe")
}
