#include "Shinobi.h"
#include <cassert>
#include <QApplication>
#include <QDateTime>

namespace shinobi {
// -------------------------------------------------------------------------- //
// Shinobi
// -------------------------------------------------------------------------- //
  Shinobi::Shinobi(QSettings* settings): mSettings(settings) {
    assert(settings != NULL);

    mTrayIcon = new QSystemTrayIcon(dynamic_cast<QApplication*>(QApplication::instance())->windowIcon(), this);
    if(!mSettings.iconHidden())
      mTrayIcon->show();
    updateTrayHint();
    connect(mTrayIcon,      SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,             SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    
    mDriveNotifier = DriveNotificationManager::instance();
    connect(mDriveNotifier, SIGNAL(driveStateChanged(int, const QString&, const QString&, const QString&)), 
            this,             SLOT(driveStateChanged(int, const QString&, const QString&, const QString&)));

    mShortcutManager = GlobalShortcutManager::instance();
    reassignHotkey();

    mMainWidget = new MainWidget(&mSettings);
    connect(mMainWidget, SIGNAL(closed()), this, SLOT(reassignHotkey()));
  }

  Shinobi::~Shinobi() {
    delete mMainWidget;
  }

  void Shinobi::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch(reason) {
    case QSystemTrayIcon::Context:
      break;
    case QSystemTrayIcon::DoubleClick:
      mMainWidget->show();
      break;
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::Unknown:
    default:
      break;
    }
  }

  void Shinobi::driveStateChanged(int stateChange, QString path, QString name, QString serialNo) {
    mSettings.addHistoryItem(HistoryItem(DriveNotificationManager::StateChange(stateChange), QDateTime::currentDateTime(), path, name, serialNo));
    mMainWidget->updateHistory();

    if(stateChange == DriveNotificationManager::Plugged) {
      foreach(const DriveFilter& driveFilter, mSettings.driveFilters())
        if(driveFilter.matches(path, name, serialNo))
          return;

      WorkerThread* worker = new WorkerThread(path, mSettings.targetPath() + "/" + QDateTime::currentDateTime().toString("yy_MM_dd_hh_mm_ss_zzz"), mSettings.fileFilters());
      //connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater())); /* TODO? We cannot do that. */
      if(!mSettings.active())
        worker->lock();
      worker->start();
      mWorkers[path] = worker;
    } else {
      assert(stateChange == DriveNotificationManager::Unplugged);

      /* Device may have been plugged before Shinobi was started. */
      if(mWorkers.contains(path)) {
        mWorkers[path]->unplug();
        if(mWorkers[path]->locked())
          mWorkers[path]->unlock();
        mWorkers.remove(path);
      }
    }
  }

  void Shinobi::toggleIconVisibility() {
    mSettings.setIconHidden(!mSettings.iconHidden());

    if(mSettings.iconHidden())
      mTrayIcon->hide();
    else
      mTrayIcon->show();
  }

  void Shinobi::toggleActivity() {
    bool active = !mSettings.active();
    mSettings.setActive(active);

    foreach(WorkerThread* worker, mWorkers)
      active ? worker->unlock() : worker->lock();

    if(active)
      QApplication::beep();

    updateTrayHint();
  }

  void Shinobi::reassignHotkey() {
    mShortcutManager->clear();
    mShortcutManager->connect(mSettings.hideHotkey(), this, SLOT(toggleIconVisibility()));
    mShortcutManager->connect(mSettings.pauseHotkey(), this, SLOT(toggleActivity()));
  }

  void Shinobi::updateTrayHint() {
    mTrayIcon->setToolTip(QString("Shinobi") + (mSettings.active() ? "" : " (paused)"));
  }
}
