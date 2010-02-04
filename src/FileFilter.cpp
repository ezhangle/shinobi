#include "FileFilter.h"
#include <cassert>
#include <QDir>
#include <QAbstractFileEngine>

namespace shinobi {
  Qt::CaseSensitivity FileFilter::fsCaseSensitivity() {
    QFileInfoList driveInfoList = QDir::drives();
    assert(driveInfoList.size() > 0);

    QAbstractFileEngine* fileEngine = QAbstractFileEngine::create(driveInfoList[0].filePath());
    bool result = fileEngine->caseSensitive();
    delete fileEngine;

    return result ? Qt::CaseSensitive : Qt::CaseInsensitive;
  }

  Qt::CaseSensitivity FileFilter::sFsCaseSensitivity = fsCaseSensitivity();

  int FileFilter::registerInQtTypeSystem() {
    int typeId = qRegisterMetaType<FileFilter>("FileFilter");
    qRegisterMetaTypeStreamOperators<FileFilter>(QMetaType::typeName(typeId));
    qRegisterMetaTypeStreamOperators<FileFilter>(QMetaType::typeName(qMetaTypeId<FileFilter>()));

    int listTypeId = qRegisterMetaType<QList<FileFilter> >("QList<FileFilter>");
    qRegisterMetaTypeStreamOperators<QList<FileFilter> >(QMetaType::typeName(listTypeId));
    qRegisterMetaTypeStreamOperators<QList<FileFilter> >(QMetaType::typeName(qMetaTypeId<QList<FileFilter> >()));

    return typeId;
  }

  int FileFilter::sQtMetaTypeId = registerInQtTypeSystem();

} // namespace shinobi


