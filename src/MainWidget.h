#ifndef __SHINOBI_MAIN_WIDGET_H__
#define __SHINOBI_MAIN_WIDGET_H__

#include "config.h"
#include <QWidget>
#include <QEvent>
#include <QTreeView>
#include <QLabel>
#include <QStandardItemModel>
#include "ShinobiSettings.h"
#include "KeySequenceEdit.h"

namespace shinobi {
  class Shinobi;

// -------------------------------------------------------------------------- //
// MainWidgetKeys
// -------------------------------------------------------------------------- //
  class MainWidgetKeys {
  public:
    static QString keySize() { return "main_widget/size"; }
    static QString keyPos () { return "main_widget/position"; }
  };


// -------------------------------------------------------------------------- //
// MainWidget
// -------------------------------------------------------------------------- //
  class MainWidget: public QWidget, private MainWidgetKeys {
    Q_OBJECT;
  public:
    MainWidget(ShinobiSettings* settings, QWidget* parent = 0);

    void updateHistory();

  signals:
    void closed();

  protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual bool eventFilter(QObject* sender, QEvent* e);

  private slots:
    void addFileFilterClicked();
    void deleteFileFilterClicked();
    void moveUpFileFilterClicked();
    void moveDownFileFilterClicked();
    void fileFilterClicked(const QModelIndex& idx);
    void addDriveFilterClicked();
    void deleteDriveFilterClicked();
    void changeTargetPath();
    void historyContextMenuRequested(const QPoint& point);
    void historyAddDriveFilter();
    void clearHistory();

  private:
    static QList<QStandardItem*> fileFilterToRow(const FileFilter& fileFilter);
    static QList<QStandardItem*> driveFilterToRow(const DriveFilter& driveFilter);
    static QList<QStandardItem*> historyItemToRow(const HistoryItem& historyItem);
    FileFilter rowToFileFilter(int row);
    DriveFilter rowToDriveFilter(int row);
    void changeCurrentIndex(QTreeView* itemView, int dRow, int dCol);

    ShinobiSettings* mSettings;

    QTreeView* mFileFilterView;
    QStandardItemModel* mFileFilterItemModel;

    QTreeView* mDriveFilterView;
    QStandardItemModel* mDriveFilterItemModel;

    QTreeView* mHistoryView;
    QStandardItemModel* mHistoryItemModel;

    QLabel* mTargetPath;
    KeySequenceEdit* mHideHotkey;

    QMenu* mHistoryContextMenu;
  };

} // namespace shinobi

#endif // __SHINOBI_MAIN_WIDGET_H__
