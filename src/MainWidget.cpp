#include "MainWidget.h"
#include <QtGui>
#include "FileFilterActionDelegate.h"

namespace shinobi {
  MainWidget::MainWidget(ShinobiSettings* settings, QWidget* parent): mSettings(settings), QWidget(parent) {
    assert(settings != NULL);

    mFileFilterView = new QTreeView();
    mFileFilterView->setRootIsDecorated(false);
    mFileFilterView->setAlternatingRowColors(true);
    mFileFilterView->setItemDelegate(new FileFilterActionDelegate());
    mFileFilterView->installEventFilter(this);
    mFileFilterView->setEditTriggers(QTreeView::EditTriggers(QTreeView::DoubleClicked | QTreeView::SelectedClicked | QTreeView::EditKeyPressed));
    
    mFileFilterItemModel = new QStandardItemModel(0, 4, this);
    mFileFilterItemModel->setHeaderData(0, Qt::Horizontal, "File");
    mFileFilterItemModel->setHeaderData(1, Qt::Horizontal, "Dir");
    mFileFilterItemModel->setHeaderData(2, Qt::Horizontal, "Mask");
    mFileFilterItemModel->setHeaderData(3, Qt::Horizontal, "Action");

    foreach(const FileFilter& fileFilter, mSettings->fileFilters())
      mFileFilterItemModel->appendRow(fileFilterToRow(fileFilter));

    mFileFilterView->setModel(mFileFilterItemModel);
    for(int i = 0; i < mFileFilterItemModel->columnCount(); i++)
      mFileFilterView->resizeColumnToContents(i);
    connect(mFileFilterView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(fileFilterClicked(const QModelIndex&)));

    QPushButton *addFileFilterButton = new QPushButton("&Add");
    connect(addFileFilterButton, SIGNAL(clicked()), this, SLOT(addFileFilterClicked()));

    QPushButton *deleteFileFilterButton = new QPushButton("&Delete");
    connect(deleteFileFilterButton, SIGNAL(clicked()), this, SLOT(deleteFileFilterClicked()));

    QPushButton *moveUpFileFilterButton = new QPushButton("Move &Up");
    connect(moveUpFileFilterButton, SIGNAL(clicked()), this, SLOT(moveUpFileFilterClicked()));

    QPushButton *moveDownFileFilterButton = new QPushButton("Move Dow&n");
    connect(moveDownFileFilterButton, SIGNAL(clicked()), this, SLOT(moveDownFileFilterClicked()));

    QHBoxLayout *fileFilterButtonsLayout = new QHBoxLayout();
    fileFilterButtonsLayout->addStretch(1);
    fileFilterButtonsLayout->addWidget(moveUpFileFilterButton);
    fileFilterButtonsLayout->addWidget(moveDownFileFilterButton);
    fileFilterButtonsLayout->addWidget(addFileFilterButton);
    fileFilterButtonsLayout->addWidget(deleteFileFilterButton);

    QVBoxLayout *fileFilterLayout = new QVBoxLayout();
    fileFilterLayout->addWidget(new QLabel("Files will be processed in the order defined by these filters."));
    fileFilterLayout->addWidget(mFileFilterView);
    fileFilterLayout->addLayout(fileFilterButtonsLayout);

    QGroupBox *fileFilterGroupBox = new QGroupBox("&File Filters");
    fileFilterGroupBox->setLayout(fileFilterLayout);

    mDriveFilterView = new QTreeView();
    mDriveFilterView->setRootIsDecorated(false);
    mDriveFilterView->setAlternatingRowColors(true);
    mDriveFilterView->installEventFilter(this);
    mDriveFilterView->setEditTriggers(QTreeView::EditTriggers(QTreeView::DoubleClicked | QTreeView::SelectedClicked | QTreeView::EditKeyPressed));

    mDriveFilterItemModel = new QStandardItemModel(0, 3, this);
    mDriveFilterItemModel->setHeaderData(0, Qt::Horizontal, "Path Mask");
    mDriveFilterItemModel->setHeaderData(1, Qt::Horizontal, "Name Mask");
    mDriveFilterItemModel->setHeaderData(2, Qt::Horizontal, "Serial# Mask");

    foreach(const DriveFilter& driveFilter, mSettings->driveFilters())
      mDriveFilterItemModel->appendRow(driveFilterToRow(driveFilter));

    mDriveFilterView->setModel(mDriveFilterItemModel);
    for(int i = 0; i < mDriveFilterItemModel->columnCount(); i++)
      mDriveFilterView->resizeColumnToContents(i);

    QPushButton *addDriveFilterButton = new QPushButton("Add");
    connect(addDriveFilterButton, SIGNAL(clicked()), this, SLOT(addDriveFilterClicked()));

    QPushButton *deleteDriveFilterButton = new QPushButton("Delete");
    connect(deleteDriveFilterButton, SIGNAL(clicked()), this, SLOT(deleteDriveFilterClicked()));

    QHBoxLayout *driveFilterButtonsLayout = new QHBoxLayout();
    driveFilterButtonsLayout->addStretch(1);
    driveFilterButtonsLayout->addWidget(addDriveFilterButton);
    driveFilterButtonsLayout->addWidget(deleteDriveFilterButton);

    QVBoxLayout *driveFilterLayout = new QVBoxLayout();
    driveFilterLayout->addWidget(new QLabel("Drives satisfying any of these filters will be ignored."));
    driveFilterLayout->addWidget(mDriveFilterView);
    driveFilterLayout->addLayout(driveFilterButtonsLayout);

    QGroupBox *driveFilterGroupBox = new QGroupBox("&Drive Filters");
    driveFilterGroupBox->setLayout(driveFilterLayout);

    QLabel* targetPathLabel = new QLabel("Target path");
    targetPathLabel->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));

    mTargetPath = new QLabel(mSettings->targetPath());
    QToolButton* openButton = new QToolButton();
    openButton->setIcon(QIcon(":/icon_open.png"));
    connect(openButton, SIGNAL(clicked()), this, SLOT(changeTargetPath()));

    QHBoxLayout* targetPathLayout = new QHBoxLayout();
    targetPathLayout->addWidget(mTargetPath, 1);
    targetPathLayout->addWidget(openButton);

    QLabel* hideHotkeyLabel = new QLabel("Hide hotkey");
    hideHotkeyLabel->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));

    mHideHotkey = new KeySequenceEdit(mSettings->hideHotkey());

    QLabel* pauseHotkeyLabel = new QLabel("Pause hotkey");
    pauseHotkeyLabel->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));

    mPauseHotkey = new KeySequenceEdit(mSettings->pauseHotkey());

    QGridLayout* generalLayout = new QGridLayout();
    generalLayout->addWidget(targetPathLabel,   0, 0);
    generalLayout->addLayout(targetPathLayout,  0, 1);
    generalLayout->addWidget(hideHotkeyLabel,   1, 0);
    generalLayout->addWidget(mHideHotkey,       1, 1);
    generalLayout->addWidget(pauseHotkeyLabel,  2, 0);
    generalLayout->addWidget(mPauseHotkey,      2, 1);

    QGroupBox *generalGroupBox = new QGroupBox("&General");
    generalGroupBox->setLayout(generalLayout);

    mSettingsSplitter = new QSplitter(Qt::Vertical);
    mSettingsSplitter->addWidget(fileFilterGroupBox);
    mSettingsSplitter->addWidget(driveFilterGroupBox);

    QVBoxLayout* settingsLayout = new QVBoxLayout();
    settingsLayout->addWidget(mSettingsSplitter);
    settingsLayout->addWidget(generalGroupBox);
    settingsLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* settingsWidget = new QWidget();
    settingsWidget->setLayout(settingsLayout);

    mHistoryView = new QTreeView();
    mHistoryView->setRootIsDecorated(false);
    mHistoryView->setAlternatingRowColors(true);
    mHistoryView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mHistoryView, SIGNAL(customContextMenuRequested(const QPoint&)), 
            this,         SLOT(historyContextMenuRequested(const QPoint&)));

    QAction* addDriveFilterAction = new QAction("&Add Drive Filter", this);
    connect(addDriveFilterAction, SIGNAL(triggered()), this, SLOT(historyAddDriveFilter()));

    mHistoryContextMenu = new QMenu(this);
    mHistoryContextMenu->addAction(addDriveFilterAction);

    mHistoryItemModel = new QStandardItemModel(0, 5, this);
    mHistoryItemModel->setHeaderData(0, Qt::Horizontal, "Time");
    mHistoryItemModel->setHeaderData(1, Qt::Horizontal, "Event");
    mHistoryItemModel->setHeaderData(2, Qt::Horizontal, "Path");
    mHistoryItemModel->setHeaderData(3, Qt::Horizontal, "Device Name");
    mHistoryItemModel->setHeaderData(4, Qt::Horizontal, "Device Serial#");

    updateHistory();

    mHistoryView->setModel(mHistoryItemModel);
    for(int i = 0; i < mHistoryItemModel->columnCount(); i++)
      mHistoryView->resizeColumnToContents(i);

    QPushButton *clearHistoryButton = new QPushButton("&Clear");
    connect(clearHistoryButton, SIGNAL(clicked()), this, SLOT(clearHistory()));

    QHBoxLayout *historyButtonsLayout = new QHBoxLayout();
    historyButtonsLayout->addStretch(1);
    historyButtonsLayout->addWidget(clearHistoryButton);

    QVBoxLayout* historyLayout = new QVBoxLayout();
    historyLayout->addWidget(mHistoryView);
    historyLayout->addLayout(historyButtonsLayout);
    //historyLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox *historyGroupBox = new QGroupBox("History");
    historyGroupBox->setLayout(historyLayout);

    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->addTab(settingsWidget,  "&Settings");
    tabWidget->addTab(historyGroupBox, "&History");

    QPushButton *closeButton = new QPushButton("&Close");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tabWidget);
    layout->addLayout(buttonsLayout);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
    setWindowTitle(QString("Shinobi v") + SHINOBI_VERSION);

    resize(mSettings->value(keySize(), QSize(640, 480)).toSize());
    move(mSettings->value(keyPos(), QApplication::desktop()->screenGeometry().center() - QPoint(640, 480) / 2).toPoint());
    
    QByteArray settingsSplitterState = mSettings->value(keySplitterState()).toByteArray();
    mSettingsSplitter->restoreState(settingsSplitterState);
  }

  QList<QStandardItem*> MainWidget::fileFilterToRow(const FileFilter& fileFilter) {
    QList<QStandardItem*> result; 

    QStandardItem* fileItem = new QStandardItem();
    fileItem->setData(fileFilter.fileTypes() & FileFilter::File ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
    fileItem->setEditable(false);

    QStandardItem* dirItem = new QStandardItem();
    dirItem->setData(fileFilter.fileTypes() & FileFilter::Directory ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
    dirItem->setEditable(false);

    QStandardItem* maskItem = new QStandardItem(fileFilter.pattern());
    maskItem->setEditable(true);

    QStandardItem* actionItem = new QStandardItem(FileFilter::actionToString(fileFilter.action()));
    actionItem->setData(fileFilter.action(), Qt::UserRole);
    actionItem->setEditable(true);

    return result << fileItem << dirItem << maskItem << actionItem;
  }

  QList<QStandardItem*> MainWidget::driveFilterToRow(const DriveFilter& driveFilter) {
    QList<QStandardItem*> result; 

    QStandardItem* pathItem = new QStandardItem(driveFilter.pathPattern());
    pathItem->setEditable(true);

    QStandardItem* nameItem = new QStandardItem(driveFilter.namePattern());
    nameItem->setEditable(true);

    QStandardItem* serialItem = new QStandardItem(driveFilter.serialNoPattern());
    serialItem->setEditable(true);

    return result << pathItem << nameItem << serialItem;
  }

  QList<QStandardItem*> MainWidget::historyItemToRow(const HistoryItem& historyItem) {
    QList<QStandardItem*> result; 

    QStandardItem* timeItem = new QStandardItem(historyItem.time().toString("dd.MM.yyyy hh:mm:ss"));
    timeItem->setEditable(false);

    QStandardItem* eventItem = new QStandardItem(HistoryItem::eventTypeToString(historyItem.eventType()));
    eventItem->setEditable(false);

    QStandardItem* pathItem = new QStandardItem(historyItem.path());
    pathItem->setEditable(false);

    QStandardItem* nameItem = new QStandardItem(historyItem.name());
    nameItem->setEditable(false);

    QStandardItem* serialNoItem = new QStandardItem(historyItem.serialNo());
    serialNoItem->setEditable(false);

    return result << timeItem << eventItem << pathItem << nameItem << serialNoItem;
  }

  FileFilter MainWidget::rowToFileFilter(int row) {
    bool fileChecked = mFileFilterItemModel->index(row, 0).data(Qt::CheckStateRole).toInt() == Qt::Checked;
    bool dirChecked = mFileFilterItemModel->index(row, 1).data(Qt::CheckStateRole).toInt() == Qt::Checked;
    QString pattern = mFileFilterItemModel->index(row, 2).data().toString();
    int action = mFileFilterItemModel->index(row, 3).data(Qt::UserRole).toInt();

    return FileFilter(FileFilter::FileTypes((fileChecked ? FileFilter::File : 0) | (dirChecked ? FileFilter::Directory : 0)), pattern, FileFilter::Action(action));
  }

  DriveFilter MainWidget::rowToDriveFilter(int row) {
    QString pathPattern = mDriveFilterItemModel->index(row, 0).data().toString();
    QString namePattern = mDriveFilterItemModel->index(row, 1).data().toString();
    QString serialNoPattern = mDriveFilterItemModel->index(row, 2).data().toString();

    return DriveFilter(pathPattern, namePattern, serialNoPattern);
  }

  void MainWidget::addFileFilterClicked() {
    mFileFilterItemModel->appendRow(fileFilterToRow(FileFilter(FileFilter::File, "*", FileFilter::Copy)));
  }

  void MainWidget::deleteFileFilterClicked() {
    QModelIndex idx = mFileFilterView->currentIndex();
    if(idx.column() == -1 || idx.row() == -1)
      return;

    mFileFilterItemModel->removeRow(idx.row());
  }

  void MainWidget::addDriveFilterClicked() {
    mDriveFilterItemModel->appendRow(driveFilterToRow(DriveFilter("", "", "")));
  }

  void MainWidget::deleteDriveFilterClicked() {
    QModelIndex idx = mDriveFilterView->currentIndex();
    if(idx.column() == -1 || idx.row() == -1)
      return;

    mDriveFilterItemModel->removeRow(idx.row());
  }

  void MainWidget::moveUpFileFilterClicked() {
    QModelIndex idx = mFileFilterView->currentIndex();
    if(idx.column() == -1 || idx.row() == -1 || idx.row() == 0)
      return;

    mFileFilterItemModel->insertRow(idx.row() - 1, mFileFilterItemModel->takeRow(idx.row()));
    mFileFilterView->setCurrentIndex(mFileFilterItemModel->index(idx.row() - 1, idx.column()));
  }

  void MainWidget::moveDownFileFilterClicked() {
    QModelIndex idx = mFileFilterView->currentIndex();
    if(idx.column() == -1 || idx.row() == -1 || idx.row() == mFileFilterItemModel->rowCount() - 1)
      return;

    mFileFilterItemModel->insertRow(idx.row() + 1, mFileFilterItemModel->takeRow(idx.row()));
    mFileFilterView->setCurrentIndex(mFileFilterItemModel->index(idx.row() + 1, idx.column()));
  }

  void MainWidget::fileFilterClicked(const QModelIndex& idx) {
    if(idx.column() == -1 || idx.row() == -1)
      return;

    if(idx.column() == 0 || idx.column() == 1) {
      Qt::CheckState current = static_cast<Qt::CheckState>(mFileFilterItemModel->data(idx, Qt::CheckStateRole).toInt());
      mFileFilterItemModel->setData(idx, current == Qt::Checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
    }
  }

  void MainWidget::closeEvent(QCloseEvent *event) {
    mSettings->setValue(keySize(), size());
    mSettings->setValue(keyPos(), pos());
    mSettings->setValue(keySplitterState(), mSettingsSplitter->saveState());

    QList<FileFilter> fileFilters;
    for(int i = 0; i < mFileFilterItemModel->rowCount(); i++)
      fileFilters.push_back(rowToFileFilter(i));
    mSettings->setFileFilters(fileFilters);

    QList<DriveFilter> driveFilters;
    for(int i = 0; i < mDriveFilterItemModel->rowCount(); i++)
      driveFilters.push_back(rowToDriveFilter(i));
    mSettings->setDriveFilters(driveFilters);

    mSettings->setTargetPath(mTargetPath->text());
    mSettings->setHideHotkey(mHideHotkey->keySequence());
    mSettings->setPauseHotkey(mPauseHotkey->keySequence());

    event->accept();

    Q_EMIT closed();
  }

  bool MainWidget::eventFilter(QObject* sender, QEvent* e) {
    bool filtered = false;
    if(e->type() == QEvent::KeyPress) {
      QKeyEvent* k = static_cast<QKeyEvent*>(e);
      switch(k->key()) {
      case Qt::Key_Delete:
        if(sender == mFileFilterView)
          deleteFileFilterClicked();
        else if(sender == mDriveFilterView)
          deleteDriveFilterClicked();
        filtered = true;
        break;
      case Qt::Key_Plus:
        if(sender == mFileFilterView)
          addFileFilterClicked();
        else if(sender == mDriveFilterView)
          addDriveFilterClicked();
        filtered = true;
        break;
      case Qt::Key_Up:
        if((k->modifiers() & ~(Qt::ControlModifier | Qt::KeypadModifier)) || !(k->modifiers() & Qt::ControlModifier))
          break;
        if(sender == mFileFilterView) {
          moveUpFileFilterClicked();
          filtered = true;
        }
        break;
      case Qt::Key_Down:
        if((k->modifiers() & ~(Qt::ControlModifier | Qt::KeypadModifier)) || !(k->modifiers() & Qt::ControlModifier))
          break;
        if(sender == mFileFilterView) {
          moveDownFileFilterClicked();
          filtered = true;
        }
        break;
      case Qt::Key_Left:
        if(sender == mFileFilterView)
          changeCurrentIndex(mFileFilterView, 0, -1);
        else if(sender == mDriveFilterView)
          changeCurrentIndex(mDriveFilterView, 0, -1);
        filtered = true;
        break;
      case Qt::Key_Right:
        if(sender == mFileFilterView)
          changeCurrentIndex(mFileFilterView, 0, 1);
        else if(sender == mDriveFilterView)
          changeCurrentIndex(mDriveFilterView, 0, 1);
        filtered = true;
        break;
      case Qt::Key_Space:
        if(sender == mFileFilterView) {
          QModelIndex idx = mFileFilterView->currentIndex();
          if(idx.column() <= 1)
            fileFilterClicked(idx);
          else
            mFileFilterView->edit(idx);
        } else if(sender == mDriveFilterView)
          mDriveFilterView->edit(mDriveFilterView->currentIndex());
        filtered = true;
        break;
      case Qt::Key_Enter:
      case Qt::Key_Return:
        if(sender == mFileFilterView)
          mFileFilterView->edit(mFileFilterView->currentIndex());
        else if(sender == mDriveFilterView)
          mDriveFilterView->edit(mDriveFilterView->currentIndex());
        filtered = true;
        break;
      default:
        break;
      }
    }
    return filtered;
  }

  void MainWidget::changeCurrentIndex(QTreeView* itemView, int dRow, int dCol) {
    QModelIndex idx = itemView->currentIndex();
    if(idx.column() == -1 || idx.row() == -1)
      return;

    QModelIndex newIdx = idx.sibling(idx.row() + dRow, idx.column() + dCol);
    if(newIdx.isValid())
      itemView->setCurrentIndex(newIdx);
  }

  void MainWidget::updateHistory() {
    const QList<HistoryItem> history = mSettings->history();

    if(history.size() < mHistoryItemModel->rowCount())
      mHistoryItemModel->removeRows(history.size(), mHistoryItemModel->rowCount() - history.size());

    for(int i = mHistoryItemModel->rowCount(); i < history.size(); i++)
      mHistoryItemModel->appendRow(historyItemToRow(history[i]));
  }

  void MainWidget::changeTargetPath() {
    QString targetPath = QFileDialog::getExistingDirectory(this, "Select target directory...", mSettings->targetPath()); 
    if(!targetPath.isEmpty())
      mTargetPath->setText(targetPath);
  }

  void MainWidget::historyContextMenuRequested(const QPoint& point) {
    QModelIndex idx = mHistoryView->indexAt(point);
    if(!idx.isValid())
      return;

    mHistoryContextMenu->popup(mHistoryView->viewport()->mapToGlobal(point));
  }

  void MainWidget::historyAddDriveFilter() {
    int row = mHistoryView->currentIndex().row();
    if(row == -1)
      return;

    HistoryItem item = mSettings->history()[row];
    if(!item.name().isEmpty() || !item.serialNo().isEmpty())
      mDriveFilterItemModel->appendRow(driveFilterToRow(DriveFilter("*", item.name(), item.serialNo())));
  }

  void MainWidget::clearHistory() {
    mSettings->clearHistory();
    updateHistory();
  }

} // namespace shinobi

