#ifndef __SHINOBI_FILE_FILTER_H__
#define __SHINOBI_FILE_FILTER_H__

#include "config.h"
#include <cassert>
#include <QList>
#include <QFlags>
#include <QRegExp>
#include <QFileInfo>
#include <QDataStream>
#include <QMetaType>

namespace shinobi {
// -------------------------------------------------------------------------- //
// FileFilter
// -------------------------------------------------------------------------- //
  class FileFilter {
  public:
    enum Action {
      Scan = 0,
      Copy = 1,
      Skip = 2,
      LastAction = Skip
    };

    enum FileType {
      Directory = 0x1,
      File = 0x2,
      Any = Directory | File
    };
    Q_DECLARE_FLAGS(FileTypes, FileType);

    static QString actionToString(Action action) {
      switch(action) {
      case Copy:
        return "Copy";
      case Scan:
        return "Scan";
      case Skip:
        return "Skip";
      default:
        Unreachable();
      }
    }

    FileFilter(): mFileTypes(0), mAction(Copy) {}

    FileFilter(FileTypes fileTypes, const QString& pattern, Action action): mFileTypes(fileTypes), mRegExp(pattern, sFsCaseSensitivity, QRegExp::Wildcard), mAction(action) {}

    bool matches(const QFileInfo& fileInfo) const {
      if(mFileTypes != Any) {
        if(fileInfo.isDir() && !(mFileTypes & Directory))
          return false;
        if(!fileInfo.isDir() && !(mFileTypes & File))
          return false;
      }

      return mRegExp.exactMatch(fileInfo.fileName());
    }

    FileTypes fileTypes() const {
      return mFileTypes;
    }

    Action action() const {
      return mAction;
    }

    QString pattern() const {
      return mRegExp.pattern();
    }

    friend QDataStream &operator<<(QDataStream &out, const FileFilter &filter) {
      return out << static_cast<int>(filter.mFileTypes) << filter.mRegExp.pattern() << filter.mAction;
    }

    friend QDataStream &operator>>(QDataStream &in, FileFilter &filter) {
      QString regexPattern;
      int fileTypes;
      int action;
      in >> fileTypes >> regexPattern >> action;
      filter.~FileFilter();
      new (&filter) FileFilter(FileTypes(fileTypes), regexPattern, Action(action));
      return in;
    }

  private:
    static Qt::CaseSensitivity fsCaseSensitivity();
    static int registerInQtTypeSystem();

    FileTypes mFileTypes;
    QRegExp mRegExp;
    Action mAction;
    static Qt::CaseSensitivity sFsCaseSensitivity;
    static int sQtMetaTypeId;
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(FileFilter::FileTypes);

} // namespace shinobi

Q_DECLARE_METATYPE(::shinobi::FileFilter);
Q_DECLARE_METATYPE(QList<::shinobi::FileFilter>);

#endif // __SHINOBI_FILE_FILTER_H__
