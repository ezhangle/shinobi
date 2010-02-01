#ifndef __SHINOBI_FILE_FILTER_ACTION_DELEGATE_H__
#define __SHINOBI_FILE_FILTER_ACTION_DELEGATE_H__

#include "config.h"
#include <QStyledItemDelegate>
#include "FileFilter.h"
#include "FileFilterActionEditor.h"

namespace shinobi {
// -------------------------------------------------------------------------- //
// FileFilterActionDelegate
// -------------------------------------------------------------------------- //
  class FileFilterActionDelegate: public QStyledItemDelegate {
    Q_OBJECT;
  public:
    FileFilterActionDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

    /*void paint(QPainter *painter, const QStyleOptionViewItem &option,
      const QModelIndex &index) const;*/
    /*QSize sizeHint(const QStyleOptionViewItem &option,
      const QModelIndex &index) const;*/
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
      if(index.column() == 3) {
        FileFilterActionEditor* editor = new FileFilterActionEditor(parent);
        /*connect(editor, SIGNAL(activated(int)), this, SIGNAL(commitData()));
        connect(editor, SIGNAL(activated(int)), this, SIGNAL(closeEditor()));*/
        return editor;
      } else
        return QStyledItemDelegate::createEditor(parent, option, index);

      /*if (qVariantCanConvert<StarRating>(index.data())) {
        StarEditor *editor = new StarEditor(parent);
        return editor;
      } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
      }*/

    }
    
    void setEditorData(QWidget *editor, const QModelIndex &index) const {
      if(index.column() == 3)
        qobject_cast<FileFilterActionEditor*>(editor)->setAction(index.data(Qt::UserRole).toInt());
      else
        QStyledItemDelegate::setEditorData(editor, index);
    }
    
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
      if(index.column() == 3) {
        int action = qobject_cast<FileFilterActionEditor*>(editor)->action();
        model->setData(index, action, Qt::UserRole);
        model->setData(index, FileFilter::actionToString(static_cast<FileFilter::Action>(action)));
      } else
        QStyledItemDelegate::setModelData(editor, model, index);
    }

#if 0
    virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& /* option */, const QModelIndex& index) {
      if(event->type() == QEvent::MouseButtonRelease) {
        bool checkState = index.model()->data(index, Qt::CheckStateRole).toBool();
        model->setData(index, !checkState);
      }
      return true; /* Event has been handled here. */
    }
#endif

/*    private slots:
      void commitAndCloseEditor();*/

  };

} // namespace shinobi


#endif // __SHINOBI_FILE_FILTER_ACTION_DELEGATE_H__
