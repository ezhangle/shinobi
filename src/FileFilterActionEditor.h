#ifndef __SHINOBI_FILE_FILTER_ACTION_EDITOR_H__
#define __SHINOBI_FILE_FILTER_ACTION_EDITOR_H__

#include "config.h"
#include <QComboBox>
#include "FileFilter.h"

namespace shinobi {
// -------------------------------------------------------------------------- //
// FileFilterActionEditor
// -------------------------------------------------------------------------- //
  class FileFilterActionEditor: public QComboBox {
    Q_OBJECT;
    Q_PROPERTY(int action READ action WRITE setAction USER true)
  public:
    FileFilterActionEditor(QWidget *parent = 0): QComboBox(parent) {
      for(int i = 0; i <= FileFilter::LastAction; ++i) {
        insertItem(i, FileFilter::actionToString(static_cast<FileFilter::Action>(i)));
        setItemData(i, i, Qt::UserRole);
      }
    }

    int action() const {
      return itemData(currentIndex(), Qt::UserRole).toInt();
    }

    void setAction(int action) {
      setCurrentIndex(findData(action, Qt::UserRole));
    }
  };

} // namespace shinobi

#endif // __SHINOBI_FILE_FILTER_ACTION_EDITOR_H__
