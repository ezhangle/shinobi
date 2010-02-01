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
    int typeId = qRegisterMetaType<FileFilter>();
    qRegisterMetaTypeStreamOperators<FileFilter>(QMetaType::typeName(typeId));
    
    int listTypeId = qRegisterMetaType<QList<FileFilter> >();
    qRegisterMetaTypeStreamOperators<QList<FileFilter> >(QMetaType::typeName(listTypeId));

    return typeId;
  }

  int FileFilter::sQtMetaTypeId = registerInQtTypeSystem();

} // namespace shinobi


