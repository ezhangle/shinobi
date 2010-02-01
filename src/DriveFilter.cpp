#include "DriveFilter.h"

namespace shinobi {
  int DriveFilter::registerInQtTypeSystem() {
    int typeId = qRegisterMetaType<DriveFilter>();
    qRegisterMetaTypeStreamOperators<DriveFilter>(QMetaType::typeName(typeId));

    int listTypeId = qRegisterMetaType<QList<DriveFilter> >();
    qRegisterMetaTypeStreamOperators<QList<DriveFilter> >(QMetaType::typeName(listTypeId));

    return typeId;
  }

  int DriveFilter::sQtMetaTypeId = registerInQtTypeSystem();

} // namespace shinobi


