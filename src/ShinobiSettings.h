#ifndef __SHINOBI_SHINOBI_SETTINGS_H__
#define __SHINOBI_SHINOBI_SETTINGS_H__

#include "config.h"
#include <cassert>
#include <QSettings>
#include "DriveFilter.h"
#include "FileFilter.h"
#include "HistoryItem.h"

namespace shinobi {
// -------------------------------------------------------------------------- //
// ShinobiSettingsKeys
// -------------------------------------------------------------------------- //
  class ShinobiSettingsKeys {
  public:
    static QString keyDriveFilters()          { return "drive_filters"; }
    static QString keyFileFilters ()          { return "file_filters"; }
    static QString keyTargetPath  ()          { return "target_path"; }
    static QString keyHideHotkey  ()          { return "hide_hotkey"; }
    static QString keyPauseHotkey ()          { return "pause_hotkey"; }
    static QString keyShowPwdHash ()          { return "show_pwd_hash"; }
    static QString keyIconHidden  ()          { return "icon_hidden"; }
    static QString keyActive      ()          { return "active"; }
    static QString keyHistorySize ()          { return "history_size"; }
    static QString keyHistoryItem (int index) { return "history/item" + QString::number(index); }
  };


// -------------------------------------------------------------------------- //
// ShinobiSettings
// -------------------------------------------------------------------------- //
  class ShinobiSettings: private ShinobiSettingsKeys {
  public:
    ShinobiSettings(QSettings* settings): mSettings(settings) {
      assert(settings != NULL);

      load(keyDriveFilters(), mDriveFilters);
      load(keyFileFilters(), mFileFilters);
      load(keyTargetPath(), mTargetPath, ".");
      
      int historySize;
      load(keyHistorySize(), historySize, 0);
      for(int i = 0; i < historySize; i++) {
        HistoryItem historyItem;
        load(keyHistoryItem(i), historyItem);
        mHistory.append(historyItem);
      }

      load(keyIconHidden(), mIconHidden, false);
      load(keyHideHotkey(), mHideHotkey, QKeySequence("Ctrl+Alt+Shift+S"));
      load(keyPauseHotkey(), mPauseHotkey, QKeySequence("Ctrl+Alt+Shift+A"));
      load(keyActive(), mActive, false);
    }

    const QList<DriveFilter> driveFilters() const {
      return mDriveFilters;
    }
    
    const QList<FileFilter> fileFilters() const {
      return mFileFilters;
    }

    const QList<HistoryItem> history() const {
      return mHistory;
    }

    QString targetPath() const {
      return mTargetPath;
    }

    bool iconHidden() const {
      return mIconHidden;
    }

    QKeySequence hideHotkey() const {
      return mHideHotkey;
    }

    QKeySequence pauseHotkey() const {
      return mPauseHotkey;
    }

    bool active() const {
      return mActive;
    }

    void setDriveFilters(const QList<DriveFilter>& driveFilters) {
      mDriveFilters = driveFilters;
      save(keyDriveFilters(), driveFilters);
    }

    void setFileFilters(const QList<FileFilter>& fileFilters) {
      mFileFilters = fileFilters;
      save(keyFileFilters(), fileFilters);
    }

    void addHistoryItem(const HistoryItem& item) {
      mHistory.append(item);
      save(keyHistorySize(), mHistory.size());
      save(keyHistoryItem(mHistory.size() - 1), item);
    }

    void clearHistory() {
      mHistory.clear();
      save(keyHistorySize(), 0);
    }

    void setTargetPath(const QString& targetPath) {
      mTargetPath = targetPath;
      save(keyTargetPath(), targetPath);
    }

    void setIconHidden(bool iconHidden) {
      mIconHidden = iconHidden;
      save(keyIconHidden(), iconHidden);
    }

    void setHideHotkey(QKeySequence keySequence) {
      mHideHotkey = keySequence;
      save(keyHideHotkey(), keySequence);
    }

    void setPauseHotkey(QKeySequence keySequence) {
      mPauseHotkey = keySequence;
      save(keyPauseHotkey(), keySequence);
    }

    void setActive(bool active) {
      mActive = active;
      save(keyActive(), active);
    }

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const {
      return mSettings->value(key, defaultValue);
    }

    void setValue(const QString &key, const QVariant &value) {
      mSettings->setValue(key, value);
    }

  private:
    ShinobiSettings(const ShinobiSettings&);
    ShinobiSettings& operator= (const ShinobiSettings&);

    template<class T>
    static QVariant variant(const T& value) {
      QVariant result;
      result.setValue(value);
      return result;
    }

    template<class T>
    void load(const QString& key, T& target) {
      target = mSettings->value(key, variant(T())).value<T>();
    }

    template<class T, class Y>
    void load(const QString& key, T& target, const Y& defaultValue) {
      target = mSettings->value(key, variant(T(defaultValue))).value<T>();
    }

    template<class T>
    void save(const QString& key, const T& value) {
      mSettings->setValue(key, variant(value));
    }

    bool mIconHidden;
    bool mActive;
    QString mTargetPath;
    QKeySequence mHideHotkey;
    QKeySequence mPauseHotkey;
    QSettings* mSettings;
    QList<DriveFilter> mDriveFilters;
    QList<FileFilter> mFileFilters;
    QList<HistoryItem> mHistory;
  };

} // namespace shinobi


#endif // __SHINOBI_SHINOBI_SETTINGS_H__
