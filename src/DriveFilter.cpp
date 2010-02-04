#include "DriveFilter.h"

namespace shinobi {
  int DriveFilter::registerInQtTypeSystem() {
    int typeId = qRegisterMetaType<DriveFilter>("DriveFilter");
    qRegisterMetaTypeStreamOperators<DriveFilter>(QMetaType::typeName(typeId));
    qRegisterMetaTypeStreamOperators<DriveFilter>(QMetaType::typeName(qMetaTypeId<DriveFilter>()));

    int listTypeId = qRegisterMetaType<QList<DriveFilter> >("QList<DriveFilter>");
    qRegisterMetaTypeStreamOperators<QList<DriveFilter> >(QMetaType::typeName(listTypeId));
    qRegisterMetaTypeStreamOperators<QList<DriveFilter> >(QMetaType::typeName(qMetaTypeId<QList<DriveFilter> >()));

    return typeId;
  }

  int DriveFilter::sQtMetaTypeId = registerInQtTypeSystem();

} // namespace shinobi


