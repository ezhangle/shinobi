#ifndef __SHINOBI_SHINOBI_H__
#define __SHINOBI_SHINOBI_H__

#include "config.h"
#include <QObject>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMap>
#include <globalshortcutmanager.h>
#include <DriveNotificationManager.h>
#include "MainWidget.h"
#include "ShinobiSettings.h"
#include "WorkerThread.h"

/* TODO: do smth with threads. */

namespace shinobi {
// -------------------------------------------------------------------------- //
// Shinobi
// -------------------------------------------------------------------------- //
  class Shinobi: public QObject {
    Q_OBJECT;
  public:
    Shinobi(QSettings* settings);
    ~Shinobi();

  private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void driveStateChanged(int stateChange, QString path, QString serialNo, QString name);
    void toggleIconVisibility();
    void reassignHotkey();
    void toggleActivity();

  private:
    void updateTrayHint();

    QSystemTrayIcon* mTrayIcon;
    MainWidget* mMainWidget;
    GlobalShortcutManager* mShortcutManager;
    DriveNotificationManager* mDriveNotifier;

    ShinobiSettings mSettings;
    QMap<QString, WorkerThread*> mWorkers;
  };

} // namespace shinobi

#endif // __SHINOBI_SHINOBI_H__
